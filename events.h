#ifndef EVENTS_H_
#define EVENTS_H_

#include "config.h"

#include <inttypes.h> // uint8_t, uint16_t

#ifdef DEV
	// Polyfills for test purpose only
	#include "../tests/polyfills/arduinoPolyfill.h"
	#include "../tests/polyfills/lcdPolyfill.h"
#endif
#ifdef PROD
	// Libraries
	#include "PCD8544.h" // PCD8544
#endif

#include "tasks.h"
#include "clock.h"
#include "navigation.h"

class Events {
	public:
		Events();

		// returns true if a menu id has been given, otherwise false
		bool displayMenu(const uint8_t navPosition, PCD8544 lcd, Clock *clk, Tasks *tasks);

		// black box which handle events
		void handle(Navigation *nav,
					PCD8544 lcd,
					Clock *clk,
					Tasks *tasks);

	private:
		// check if pin is active and not on cooldown
		bool checkPin(const int pinId);

		void blinkCharacter(const bool unblinkIt,
							Navigation *nav,
							PCD8544 lcd,
							Clock *clk,
							Tasks *tasks);

		// little helper to extract char with the selected mask
		uint8_t extractTimeNumberToChar(const mtime_t time, const mtime_t mask);

		// a big switch for time change value (on clock or scheddule menu)
		void dispatchValueChange(Navigation *nav, PCD8544 lcd, Clock *clk, Tasks *tasks);


		// used to avoid keypress repetition
		uint16_t cooldownList[PIN_STOCK_INDICATOR_O];

		// true by default as long as we display a hello screen first
		bool screenState;
};

#endif /* EVENTS_H_ */