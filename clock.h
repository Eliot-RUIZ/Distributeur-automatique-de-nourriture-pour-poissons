#ifndef CLOCK_H_
#define CLOCK_H_

#include <inttypes.h> // uint16_t, uint32_t

#include <iostream>

/*---------------------------------------------------------------------------------------------*
 * The logic is based upon the HhMm mask, which means the hour is easily readable.             *
 *---------------------------------------------------------------------------------------------*
 * The hours format is Xx00 and extracted by 100 and filtered by the mod which is generaly 24. *
 * So hours are extracted like 2534 / 100 = 25 => 25 % 24 = 1.                                 *
 * Then we give it back its weight by 1 * 100 = 100 and add the minutes 100 + 34 = 134.        *
 * New mtime_t is 134 which is a pretty readable equivalent for => 01:34.                      *
 *                                                                                             *
 * The minutes format is Xx, hours extraction is needed and also a filter by 60.               *
 * So hours need to be extracted like 2267 / 100 * 100 = 2200.                                 *
 * Then we can extract minutes 2267 - 2200 = 67 and we round it by 67 % 60 = 7.                *
 * We finally put hours back 7 + 2200 = 2207.                                                  *
 * New mtime_t is 2207 which is again a pretty readable equivalent for => 22:07.               *
 *---------------------------------------------------------------------------------------------*
 * Specs:                                                                                      *
 * mtime_t >= 0                                                                                *
 * mtime_t <= 2359                                                                             *
 * mtime_t <= Xx59                                                                             *
 *---------------------------------------------------------------------------------------------*/
typedef int16_t mtime_t;

#define MILLIS_PER_SECOND (1024) // millis() seems to have a ~1.024ms precision (apart the rest)
#define SECOND_PER_MINUTE (60)
#define MINUTE_PER_HOUR (60)
#define HOUR_PER_DAY (24)

#define MILLIS_PER_MINUTE (61440)
#define MILLIS_PER_HOUR (3686400)

class Clock {

	public:
		Clock();

		// function call which update the time given by millis() function
		void updateTime(const uint32_t currentTime);

		// public utility function to change the time depending on the HhMm pattern
		mtime_t utilityExtractDigit(const mtime_t focus, const mtime_t mask);
		mtime_t utilityOverwriteDigit(const mtime_t focus, const mtime_t digit, const mtime_t mask);
		mtime_t utilityChangeTime(const mtime_t time, const mtime_t mask, const mtime_t hour_mod);

		// update the time based on HhMm pattern
		void changeTime(const mtime_t update);

		// get stocked time value, HhMm pattern
		mtime_t getTime();
		uint32_t getPreviousTime();
		uint32_t getMod();

		// return true and set private hasChanged value at false
		// if any change has been made recently
		bool toggleChange();

	private:

		// stocked time, HhMm pattern
		mtime_t privateTime;

		// previous time given by millis
		uint32_t previousTime;

		// change indicator semaphore-like
		bool hasChanged;

		// the clock delay value
		bool hasReset;
		uint32_t mod;

};

#endif /* CLOCK_H_ */