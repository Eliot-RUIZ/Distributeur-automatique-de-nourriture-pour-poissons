/*
* @Author: Rémy Ruiz
* @Date:   2015-08-01 10:49:36
* @Last Modified by:   Rémy Ruiz
* @Last Modified time: 2015-09-01 16:57:29
*/
#include "tasks.h"
#include "config.h"

#ifdef DEV
	// Polyfills for test purpose only
	#include "../tests/polyfills/arduinoPolyfill.h"
#endif
#ifdef PROD
	// Libraries
	#if ARDUINO < 100
		#include <WProgram.h> // delay()
	#else
		#include <Arduino.h> // delay()
	#endif
#endif

#include "clock.h"
#include "stepper_motor.h"

Tasks::Tasks() {
	// initializing lists to respect the 0 < 1 < 2 rule
	this->list[0].time = 100; // midnight
	this->list[0].maxLimit = MAX_FOOD_VALUE_C1; // the maximum level of the food
	this->list[0].alertLimit = ALERT_LIMIT_VALUE_C1; // the level below which the alert is triggered
	this->list[0].level = 0; // default is empty box
	this->list[0].done = false;

	this->list[1].time = 200; // 1AM
	this->list[1].maxLimit = MAX_FOOD_VALUE_C2;
	this->list[1].alertLimit = ALERT_LIMIT_VALUE_C2;
	this->list[1].level = 0;
	this->list[1].done = false;

	this->list[2].time = 300; // 2AM
	this->list[2].maxLimit = MAX_FOOD_VALUE_C3;
	this->list[2].alertLimit = ALERT_LIMIT_VALUE_C3;
	this->list[2].level = 0;
	this->list[2].done = false;
}

bool Tasks::updateTaskTime(const int id, const mtime_t update, Clock *clk) {
	// static is here a small optimisation
	// as long as we would never export it
	mtime_t save;

	if (id >= 0 && id < TASKS_LIST_SIZE) {
		save = this->list[id].time;

		// the hardware rule hour1 < hour2 < hour3
		if (id < TASKS_LIST_SIZE - 1) {
			// max value is the next hour this->list[id + 1].time
			this->list[id].time = clk->utilityChangeTime(this->list[id].time, update, this->list[id + 1].time / 100);
		} else {
			this->list[id].time = clk->utilityChangeTime(this->list[id].time, update, HOUR_PER_DAY);
		}

		// roll back test (inverse update * -1)
		if (id > 0 && (this->list[id].time <= this->list[id - 1].time)) {
			this->list[id].time = clk->utilityOverwriteDigit(this->list[id].time,
									clk->utilityExtractDigit(this->list[id - 1].time, update)
										+ (update > 0 ? update : update * -1), update);
		}

		switch(id) {
			case 0:
				if (this->list[id].time >= this->list[id + 1].time) {
					this->list[id].time = save;
					return (false);
				}
				break;
			case TASKS_LIST_SIZE - 1:
				if (this->list[id].time <= this->list[id - 1].time
				|| this->list[id].time >= HOUR_PER_DAY * 100) {
					this->list[id].time = save;
					return (false);
				}
				break;
			default:
				if (this->list[id].time <= this->list[id - 1].time
				|| this->list[id].time >= this->list[id + 1].time) {
					this->list[id].time = save;
					return (false);
				}
				break;
		}

		// something changed
		return (true);
	}

	// we did not change anything
	return (false);
}

int Tasks::getTaskTime(const int id) {
	if (id >= 0 && id <= TASKS_LIST_SIZE) {
		return (this->list[id].time);
	}
	return (-1);
}

bool Tasks::resolveTasks(Clock *clk, StepperMotor stepper) {
	if (this->list[0].time == clk->getTime()) {
		// already done once ? don't do it
		if (this->list[0].done)
			return (false);

		stepper.step(683);

		// decrease the food
		decreaseFoodLevel(0);

		// already done once, remember it
		this->list[0].done = true;
		return (true);

	} else if (this->list[1].time == clk->getTime()) {
		// already done once ? don't do it
		if (this->list[1].done)
			return (false);

		stepper.step(682);

		// decrease the food
		decreaseFoodLevel(1);

		// already done once, remember it
		this->list[1].done = true;
		return (true);

	} else if (this->list[2].time == clk->getTime()) {
		// already done once ? don't do it
		if (this->list[2].done)
			return (false);

		stepper.step(683);

		delay(10000);
		stepper.step(2048);

		// decrease the food
		decreaseFoodLevel(2);

		// already done once, remember it
		this->list[2].done = true;
		return (true);

	// no task to run ? reset the tokens
	} else {
		this->list[0].done = false;
		this->list[1].done = false;
		this->list[2].done = false;
	}

	// we didn't find a task to launch
	return (false);
}

uint8_t Tasks::getActualFoodLevel(const int id) {
	if (id >= 0 && id < TASKS_LIST_SIZE) {
		if (this->list[id].level == 0) {
			return 'X';
		} else if (this->list[id].level <= this->list[id].alertLimit) {
			return 'O';
		} else {
			return 'V';
		}
	}
	// error
	return 'E';
}

void Tasks::decreaseFoodLevel(const int id) {
	if (id >= 0 && id < TASKS_LIST_SIZE
		&& this->list[id].level > 0) {
		this->list[id].level--;
	}
}

void Tasks::resetFoodLevel(const int id) {
	if (id >= 0 && id < TASKS_LIST_SIZE) {
		this->list[id].level = this->list[id].maxLimit;
	}
}

bool Tasks::checkFoodLevels() {
	for (int i = 0; i < TASKS_LIST_SIZE; ++i) {
		if (this->list[i].level <= this->list[i].alertLimit) {
			// some level is not high enough
			return (false);
		}
	}
	// nothing to worry about, all food levels are high
	return (true);
}

