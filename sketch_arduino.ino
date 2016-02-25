/*
* @Author: Rémy Ruiz
* @Date:   2015-08-01 10:49:36
* @Last Modified by:   Rémy Ruiz
* @Last Modified time: 2015-09-01 17:23:06
*/
#include "config.h"

#ifdef DEV
	// Polyfills for test purpose only
	#include "../tests/polyfills/arduinoPolyfill.h"
	#include "../tests/polyfills/lcdPolyfill.h"
#endif
#ifdef PROD
	// Libraries
	#include "PCD8544.h" // PCD8544
	#if ARDUINO < 100
		#include <WProgram.h> // delay(), pinMode(), digitalWrite()
	#else
		#include <Arduino.h> // delay(), pinMode(), digitalWrite()
	#endif
#endif

#include "navigation.h"
#include "tasks.h"
#include "clock.h"
#include "events.h"
#include "stepper_motor.h"

// global objects
PCD8544 lcd;
StepperMotor stepper;

Navigation *nav = new Navigation;
Clock *clk = new Clock;
Tasks *tasks = new Tasks;
Events *events = new Events;

uint32_t blinkTime = 0;

void setup() {
	// every _I terminated pin constant is an input
	pinMode(PIN_DIRECTION_DOWN_I,	INPUT);
	pinMode(PIN_DIRECTION_RIGHT_I,	INPUT);
	pinMode(PIN_RESET_VALUE_I,		INPUT);
	pinMode(PIN_SWITCH_LIGHT_I,		INPUT);

	// every _O terminated pin constant is an output
	pinMode(PIN_STOCK_INDICATOR_O,	OUTPUT);

	// guess what, we start the screen
	lcd.begin(SCREEN_X, SCREEN_Y);

	// attach the stepper motor
    stepper.attach(STEPPER_IN1, STEPPER_IN2, STEPPER_IN3, STEPPER_IN4);

	// A hello screen on setup
	lcd.setCursor(3, 2);
	lcd.print("BONJOUR!");
	delay(MILLIS_PER_SECOND * 3);

	// display clock menu
	events->displayMenu(0, lcd, clk, tasks);
}

// TODO: test this : doesn't work well on paper
static void blinkTheIndicator() {
	// blink it every BLINK_SPEED time
	if (blinkTime >= BLINK_SPEED) {
		blinkTime = 0;
		if (digitalRead(PIN_STOCK_INDICATOR_O) == HIGH) {
			// lcd.setCursor(4, 21);
			// lcd.print("    ");
			digitalWrite(PIN_STOCK_INDICATOR_O, LOW);
		} else {
			// lcd.setCursor(4, 21);
			// lcd.print("HIGH");
			digitalWrite(PIN_STOCK_INDICATOR_O, HIGH);
		}
	}
	blinkTime++;
}

void loop() {
	// update the time
	clk->updateTime(millis());

	// according to tasks.h, if nothing wrong it returns true
	if (!tasks->checkFoodLevels()) {
		blinkTheIndicator();
	} else if (digitalRead(PIN_STOCK_INDICATOR_O) == HIGH) {
		// lcd.setCursor(4, 21);
		// lcd.print("    ");
		// switch off the blinking led
		digitalWrite(PIN_STOCK_INDICATOR_O, LOW);
	}

	// check if any task has to be done
	// if so, jump to another loop turn
	if (!tasks->resolveTasks(clk, stepper)) {
		events->handle(nav, lcd, clk, tasks);
	}
}
