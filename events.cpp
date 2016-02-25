/*
* @Author: Rémy Ruiz
* @Date:   2015-08-01 10:49:35
* @Last Modified by:   Rémy Ruiz
* @Last Modified time: 2015-09-01 17:23:06
*/

#include "config.h"

#include <inttypes.h> // uint8_t

#ifdef DEV
	// Polyfills for test purpose only
	#include "../tests/polyfills/arduinoPolyfill.h"
	#include "../tests/polyfills/lcdPolyfill.h"
#endif
#ifdef PROD
	// Libraries
	#include "PCD8544.h" // PCD8544
	#if ARDUINO < 100
		#include <WProgram.h> // digitalRead()
	#else
		#include <Arduino.h> // digitalRead()
	#endif
#endif

#include "events.h"
#include "tasks.h"
#include "clock.h"
#include "navigation.h"

Events::Events() {
	// TODO: check if the screen state need to be true or false by default
	this->screenState = true;

	// this->screenState = true;
	for (int i = 0; i < PIN_STOCK_INDICATOR_O; ++i) {
		this->cooldownList[i] = 0;
	}
}

void Events::blinkCharacter(const bool unblinkIt, Navigation *nav, PCD8544 lcd, Clock *clk, Tasks *tasks) {

	char currentChar;
	static bool state = false;
	static unsigned long prevMillis = 0;
	unsigned long curMillis = millis();

	if (unblinkIt || curMillis - prevMillis > CURSOR_BLINK_SPEED) {
		switch (nav->getCursor()->id) {

			case 0:
			case 5:
				currentChar = 'M';
				break;
			case 6:
			case 11:
			case 16:
				currentChar = 'C';
				break;

			// general clock update
			case 1:
				currentChar = extractTimeNumberToChar(clk->getTime(), 1000);
				break;
			case 2:
				currentChar = extractTimeNumberToChar(clk->getTime(),  100);
				break;
			case 3:
				currentChar = extractTimeNumberToChar(clk->getTime(),   10);
				break;
			case 4:
				currentChar = extractTimeNumberToChar(clk->getTime(),    1);
				break;

			// 1st food box
			case 7:
				currentChar = extractTimeNumberToChar(tasks->getTaskTime(0), 1000);
				break;
			case 8:
				currentChar = extractTimeNumberToChar(tasks->getTaskTime(0),  100);
				break;
			case 9:
				currentChar = extractTimeNumberToChar(tasks->getTaskTime(0),   10);
				break;
			case 10:
				currentChar = extractTimeNumberToChar(tasks->getTaskTime(0),    1);
				break;

			// 2nd food box
			case 12:
				currentChar = extractTimeNumberToChar(tasks->getTaskTime(1), 1000);
				break;
			case 13:
				currentChar = extractTimeNumberToChar(tasks->getTaskTime(1),  100);
				break;
			case 14:
				currentChar = extractTimeNumberToChar(tasks->getTaskTime(1),   10);
				break;
			case 15:
				currentChar = extractTimeNumberToChar(tasks->getTaskTime(1),    1);
				break;

			// 3rd food box
			case 17:
				currentChar = extractTimeNumberToChar(tasks->getTaskTime(2), 1000);
				break;
			case 18:
				currentChar = extractTimeNumberToChar(tasks->getTaskTime(2),  100);
				break;
			case 19:
				currentChar = extractTimeNumberToChar(tasks->getTaskTime(2),   10);
				break;
			case 20:
				currentChar = extractTimeNumberToChar(tasks->getTaskTime(2),    1);
				break;
		}
	}

	if (unblinkIt) {
		if (currentChar != 'C')
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
		else
			lcd.setCursor(nav->getCursor()->x - 1, nav->getCursor()->y);
		lcd.write(currentChar);

		// exit
		return;
	}

	if (curMillis - prevMillis > CURSOR_BLINK_SPEED) {
		if (currentChar != 'C')
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
		else
			lcd.setCursor(nav->getCursor()->x - 1, nav->getCursor()->y);
		lcd.write(state ? currentChar : ' ');
		state = !state;

		// update the saved time
		prevMillis = curMillis;
	}
}

bool Events::checkPin(const int pinId) {
	// check if action has been triggered
	// and is not on cooldown
	if (digitalRead(pinId) == HIGH && this->cooldownList[pinId] <= 0) {

		// we set a cooldown on the action
		// for it doesn't repeat too fast
		this->cooldownList[pinId] = KEYPRESS_COOLDOWN_VALUE;
		return (true);
	}
	return (false);
}

uint8_t Events::extractTimeNumberToChar(const mtime_t focus, const mtime_t mask) {
	if (focus == 0) return ('0');

	// focus / abs(mask) % 10 + char convertion
	// we use abs(mask) by pure security concern
	// ex: focus = 2345 ; mask =  100
	// we clearly want the 3 digit converted to '3'

		// example in action :
		// 2345 / abs(100)
		// = 23 % 10
		// = 3 + '0'
		// = '3'

	return (focus / (mask > 0 ? mask : mask * -1) % 10 + '0');
}

void Events::dispatchValueChange(Navigation *nav, PCD8544 lcd, Clock *clk, Tasks *tasks) {
	switch (nav->getCursor()->id) {

		// general clock update
		case 1:
			clk->changeTime(1000);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(clk->getTime(), 1000));
			break;
		case 2:
			clk->changeTime( 100);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(clk->getTime(),  100));
			break;
		case 3:
			clk->changeTime(  10);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(clk->getTime(),   10));
			break;
		case 4:
			clk->changeTime(   1);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(clk->getTime(),    1));
			break;

		// 1st food box
		case 7:
			tasks->updateTaskTime(0, 1000, clk);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(tasks->getTaskTime(0), 1000));
			break;
		case 8:
			tasks->updateTaskTime(0,  100, clk);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(tasks->getTaskTime(0),  100));
			break;
		case 9:
			tasks->updateTaskTime(0,   10, clk);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(tasks->getTaskTime(0),   10));
			break;
		case 10:
			tasks->updateTaskTime(0,    1, clk);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(tasks->getTaskTime(0),    1));
			break;

		// 2nd food box
		case 12:
			tasks->updateTaskTime(1, 1000, clk);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(tasks->getTaskTime(1), 1000));
			break;
		case 13:
			tasks->updateTaskTime(1,  100, clk);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(tasks->getTaskTime(1),  100));
			break;
		case 14:
			tasks->updateTaskTime(1,   10, clk);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(tasks->getTaskTime(1),   10));
			break;
		case 15:
			tasks->updateTaskTime(1,    1, clk);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(tasks->getTaskTime(1),    1));
			break;

		// 3rd food box
		case 17:
			tasks->updateTaskTime(2, 1000, clk);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(tasks->getTaskTime(2), 1000));
			break;
		case 18:
			tasks->updateTaskTime(2,  100, clk);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(tasks->getTaskTime(2),  100));
			break;
		case 19:
			tasks->updateTaskTime(2,   10, clk);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(tasks->getTaskTime(2),   10));
			break;
		case 20:
			tasks->updateTaskTime(2,    1, clk);
			lcd.setCursor(nav->getCursor()->x, nav->getCursor()->y);
			lcd.write(extractTimeNumberToChar(tasks->getTaskTime(2),    1));
			break;
	}
}

bool Events::displayMenu(const uint8_t navPosition, PCD8544 lcd, Clock *clk, Tasks *tasks) {
	char lineBuffer[13];

	// display clock menu
	if (navPosition == 0) {

		// clear the screen
		lcd.clear();

		// menu title
		lcd.setCursor(3, 0); lcd.print("Horloge:");
		// menu cursor indicator
		lcd.setCursor(0, 2); lcd.write('M');
		// TODO: put some contrast because the cursor now is here

		// display clock
		// TODO: check this as optimization
		lineBuffer[0] = extractTimeNumberToChar(clk->getTime(), 1000);
		lineBuffer[1] = extractTimeNumberToChar(clk->getTime(),  100);
		lineBuffer[2] = ':';
		lineBuffer[3] = extractTimeNumberToChar(clk->getTime(),   10);
		lineBuffer[4] = extractTimeNumberToChar(clk->getTime(),    1);
		lineBuffer[5] = 'H';
		lineBuffer[6] = 0;
		lcd.setCursor(4, 3); lcd.print(lineBuffer);

		return (true);
	// display scheddule menu
	} else if (navPosition == 5) {

		// clear the screen
		lcd.clear();

		// menu title
		lcd.setCursor(1, 0); lcd.print("Heures Repas");
		lcd.setCursor(1, 1); lcd.print("Des Poissons");
		// menu cursor indicator
		lcd.setCursor(0, 3); lcd.write('M');

		// 1st line
		lineBuffer[0] = 'C';
		lineBuffer[1] = '1';
		lineBuffer[2] = '[';
		lineBuffer[3] = tasks->getActualFoodLevel(0);
		lineBuffer[4] = ']';
		lineBuffer[5] = extractTimeNumberToChar(tasks->getTaskTime(0), 1000);
		lineBuffer[6] = extractTimeNumberToChar(tasks->getTaskTime(0),  100);
		lineBuffer[7] = ':';
		lineBuffer[8] = extractTimeNumberToChar(tasks->getTaskTime(0),   10);
		lineBuffer[9] = extractTimeNumberToChar(tasks->getTaskTime(0),    1);
		lineBuffer[10] = 'H';
		lineBuffer[11] = 0;
		lcd.setCursor(2, 3); lcd.print(lineBuffer);

		// 2nd line
		lineBuffer[0] = 'C';
		lineBuffer[1] = '2';
		lineBuffer[2] = '[';
		lineBuffer[3] = tasks->getActualFoodLevel(1);
		lineBuffer[4] = ']';
		lineBuffer[5] = extractTimeNumberToChar(tasks->getTaskTime(1), 1000);
		lineBuffer[6] = extractTimeNumberToChar(tasks->getTaskTime(1),  100);
		lineBuffer[7] = ':';
		lineBuffer[8] = extractTimeNumberToChar(tasks->getTaskTime(1),   10);
		lineBuffer[9] = extractTimeNumberToChar(tasks->getTaskTime(1),    1);
		lineBuffer[10] = 'H';
		lineBuffer[11] = 0;
		lcd.setCursor(2, 4); lcd.print(lineBuffer);

		// 3rd line
		lineBuffer[0] = 'C';
		lineBuffer[1] = '3';
		lineBuffer[2] = '[';
		lineBuffer[3] = tasks->getActualFoodLevel(2);
		lineBuffer[4] = ']';
		lineBuffer[5] = extractTimeNumberToChar(tasks->getTaskTime(2), 1000);
		lineBuffer[6] = extractTimeNumberToChar(tasks->getTaskTime(2),  100);
		lineBuffer[7] = ':';
		lineBuffer[8] = extractTimeNumberToChar(tasks->getTaskTime(2),   10);
		lineBuffer[9] = extractTimeNumberToChar(tasks->getTaskTime(2),    1);
		lineBuffer[10] = 'H';
		lineBuffer[11] = 0;
		lcd.setCursor(2, 5); lcd.print(lineBuffer);

		return (true);
	}
	return (false);
}

void Events::handle(Navigation *nav, PCD8544 lcd, Clock *clk, Tasks *tasks) {
	// static is here a small optimisation
	// as long as we would never export it
	static navDirection direction;

	// decrease every cooldown sets
	for (uint16_t i = 0; i < PIN_STOCK_INDICATOR_O; ++i) {
		if (this->cooldownList[i] > 0) {
			this->cooldownList[i]--;
		}
	}

	// check the registered screen power status
	if (this->screenState) {

		blinkCharacter(false, nav, lcd, clk, tasks);

		/*
		 * 4 button event handle here
		 */
		// navigate down or change value
		if (checkPin(PIN_DIRECTION_DOWN_I)) {

			// clear the cursor of its blinking
			blinkCharacter(true, nav, lcd, clk, tasks);

			if (nav->navigate(direction = DOWN)) {
				displayMenu(nav->getCursor()->id, lcd, clk, tasks);

			} else {
				// if navigation handler fails, it means the action
				// to do is updating value pointed by cursor
				dispatchValueChange(nav, lcd, clk, tasks);
			}

		}

		// navigate right
		if (checkPin(PIN_DIRECTION_RIGHT_I)) {
			// clear the cursor of its blinking
			blinkCharacter(true, nav, lcd, clk, tasks);
			nav->navigate(direction = RIGHT);
		}

		// reset food level
		if (checkPin(PIN_RESET_VALUE_I)) {

			// if cursor on 1st food scheddule id=0
			if (nav->getCursor()->id >= 6
				&& nav->getCursor()->id <= 10) {
				tasks->resetFoodLevel(0);
				lcd.setCursor(5, 3); lcd.write(tasks->getActualFoodLevel(0));

			// if cursor on 2nd food scheddule id=1
			} else if (nav->getCursor()->id >= 11
				&& nav->getCursor()->id <= 15) {
				tasks->resetFoodLevel(1);
				lcd.setCursor(5, 4); lcd.write(tasks->getActualFoodLevel(1));

			// if cursor on 3rd food scheddule id=2
			} else if (nav->getCursor()->id >= 16
				&& nav->getCursor()->id <= 20) {
				tasks->resetFoodLevel(2);
				lcd.setCursor(5, 5); lcd.write(tasks->getActualFoodLevel(2));
			}

		}

		// shutdown screen
		if (checkPin(PIN_SWITCH_LIGHT_I)) {
			// shut down the screen
			lcd.setPower(false);
			this->screenState = false;
			// then reset the cursor position
			nav->resetCursorPosition();

		}

		// update the clock's visual
		if (nav->getCursor()->id < 5
			&& clk->toggleChange()) {
			// displayMenu(nav->getCursor()->id, lcd, clk, tasks);

			// if the cursor id is below 5, it means we are currently
			// viewing the clock menu and if and update has been made on the clock
			// we have to update the clock
			lcd.setCursor(4, 3); lcd.write(extractTimeNumberToChar(clk->getTime(), 1000));
			lcd.setCursor(5, 3); lcd.write(extractTimeNumberToChar(clk->getTime(),  100));
			// empty space filled with ':'
			lcd.setCursor(7, 3); lcd.write(extractTimeNumberToChar(clk->getTime(),   10));
			lcd.setCursor(8, 3); lcd.write(extractTimeNumberToChar(clk->getTime(),    1));
		}

	} else if (checkPin(PIN_SWITCH_LIGHT_I)) {
		// light up the screen
		lcd.setPower(true);
		this->screenState = true;

		displayMenu(nav->getCursor()->id, lcd, clk, tasks);
	}

}
