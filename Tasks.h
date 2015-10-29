#ifndef TASKS_H_
#define TASKS_H_

#include "config.h"

#include <inttypes.h> // uint8_t

#ifdef DEV
	// Polyfills for test purpose only
	#include "../tests/polyfills/arduinoPolyfill.h"
#endif

#include "clock.h"
#include "stepper_motor.h"

struct task {
	mtime_t time;
	uint16_t maxLimit;
	uint16_t alertLimit;
	uint16_t level;
	bool done;
};

class Tasks {

	public:
		Tasks();

		// update task at the selected id, returns false on error
		bool updateTaskTime(const int id, const mtime_t update, Clock *clk);

		// returns the time of the requested task id
		int getTaskTime(const int id);

		// true or false depending on wether or not any task has been executed
		bool resolveTasks(Clock *clk, StepperMotor stepper);

		// returns 'E' on error, 'O', 'V' or 'X' on success
		uint8_t getActualFoodLevel(const int id);

		// function which allows us to change food level
		void resetFoodLevel(const int id);

		// true is ok, false is not ok
		bool checkFoodLevels();

	private:
		void decreaseFoodLevel(const int id);

		// the list of tasks
		// can accept up to TASKS_LIST_SIZE variable of struct task type
		struct task list[TASKS_LIST_SIZE];
};

#endif /* TASKS_H_ */
