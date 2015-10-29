#include "clock.h"

#include <inttypes.h> // uint32_t

#include <iostream>

Clock::Clock() {
	this->privateTime = 0;
	this->previousTime = 0;
	this->hasChanged = false;
	this->hasReset = true;
}

/*
 * Function below follows mtime_t type logic (cf. clock.h)
 * The logic of this code is based on the MILLIS_PER_MINUTE macro.
 * That's because the time given by the function millis() is in millisecond.
 */
void Clock::updateTime(const uint32_t currentTime) {
	// get the total minutes delta until last check
	uint32_t delta = (currentTime - this->previousTime) / MILLIS_PER_MINUTE;

	// makes update every minute (or more) only
	if (delta > 0) {

		if (this->hasReset) {
			// test if whether or not the timer has been reset
			if (currentTime == 0 && this->previousTime != 0) {
				this->previousTime = 0;
				this->hasReset = false;

				// the update has been made so we quit
				// we may loose 1 cycle doing that but
				// since we update the time each minute it's fine
				return;
			}
		} else {
			this->hasReset = true;
		}

		this->privateTime += delta;
		// if a minute has passed, increase hours and reset minutes
		if ((this->privateTime - (this->privateTime / 100 * 100)) >= MINUTE_PER_HOUR) {
			this->privateTime = ((this->privateTime / 100 + 1) % HOUR_PER_DAY) * 100;
		}

		this->previousTime = currentTime;
		this->hasChanged = true;
	}
}

mtime_t Clock::utilityExtractDigit(const mtime_t focus, const mtime_t mask) {
	if (focus == 0) return (0);
	return (focus / (mask > 0 ? mask : mask * -1) % 10 * (mask > 0 ? mask : mask * -1));
}

mtime_t Clock::utilityOverwriteDigit(const mtime_t focus, const mtime_t digit, const mtime_t mask) {
	// we need to extract only the concerned digit and replace it
	// whatever the mask is positive or negative value
	return (focus - utilityExtractDigit(focus, mask) + utilityExtractDigit(digit, mask));
}

/*
 * Function below follows mtime_t type logic (cf. clock.h)
 * Only the mask selected digit changes, if minutes are exceeded, it doesn't up hours.
 */
mtime_t Clock::utilityChangeTime(const mtime_t thisTime, const mtime_t mask, const mtime_t hour_mod) {
	// since we can make negative masks we do need to check those too
	// hour_mod also has to be not null
	if ((hour_mod && mask < 10000 && mask >= 100)
	|| (mask > -10000 && mask <= -100)) { // change hours

		if (mask < 0 && thisTime < mask * -1) {
			// this is magic reverse counting
			if ((hour_mod / 10 * 1000) + thisTime > (hour_mod * 100)) {
				return utilityOverwriteDigit(thisTime, ((hour_mod / 10 * 1000) + thisTime + mask), mask);
			} else {
				return utilityOverwriteDigit(thisTime, ((hour_mod / 10 * 1000) + thisTime), mask);
			}

		} else if (utilityExtractDigit(thisTime, mask) == 0 && mask < 0) {
			// 2029 - 100 special case
			return utilityOverwriteDigit(thisTime, (((hour_mod) - (thisTime / 100)) * 100) - 1, mask);
		} else {
			return utilityOverwriteDigit(thisTime, (thisTime + mask) % (hour_mod * 100), mask);
		}

	} else if (mask < 100 && mask > -100) { // change minutes

		if (mask < 0 && thisTime < mask * -1) {
			// this is magic reverse counting
			if (MINUTE_PER_HOUR + thisTime > MINUTE_PER_HOUR) {
				return utilityOverwriteDigit(thisTime, (MINUTE_PER_HOUR + thisTime + mask), mask);
			} else {
				return utilityOverwriteDigit(thisTime, (MINUTE_PER_HOUR + thisTime), mask);
			}
		} else {
			return utilityOverwriteDigit(thisTime, ((thisTime - (thisTime / 100 * 100)) + mask) % MINUTE_PER_HOUR, mask);
		}

	}
	return (thisTime);
}

void Clock::changeTime(const mtime_t update) {
	this->privateTime = utilityChangeTime(this->privateTime, update, HOUR_PER_DAY);
}

mtime_t Clock::getTime() {
	return this->privateTime;
}

uint32_t Clock::getPreviousTime() {
	return this->previousTime;
}

uint32_t Clock::getMod() {
	return this->mod;
}

bool Clock::toggleChange() {
	if (this->hasChanged) {
		this->hasChanged = false;
		return (true);
	}
	return (false);
}
