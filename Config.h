#ifndef CONFIG_H_
#define CONFIG_H_

#define PROD true

#include <inttypes.h> // uint16_t

#include "clock.h"

/*
 * Global configuration
 */
// INPUTS
const int PIN_DIRECTION_DOWN_I = 22;
const int PIN_DIRECTION_RIGHT_I = 26;
const int PIN_RESET_VALUE_I = 11;
const int PIN_SWITCH_LIGHT_I = 10;

// OUTPUTS
const int PIN_STOCK_INDICATOR_O = 52;

const int LCD_RST = 6;
const int LCD_CE = 7;
const int LCD_DC = 5;
const int LCD_DIN = 4;
const int LCD_CLK = 3;

const int STEPPER_IN1 = 13;
const int STEPPER_IN2 = 12;
const int STEPPER_IN3 = 9;
const int STEPPER_IN4 = 8;

// screen size
const int SCREEN_X = 84;
const int SCREEN_Y = 48;

// LED food level indicator blink speed
const int BLINK_SPEED = 1000;
const int CURSOR_BLINK_SPEED = 500;

/*
 * Events operation configuration
 */
const uint16_t KEYPRESS_COOLDOWN_VALUE = 1500;

/*
 * Tasks operation configuration
 */

const mtime_t TASKS_LIST_SIZE = 3; // the number of different tasks

const mtime_t MAX_FOOD_VALUE_C1 = 10; // C1
const mtime_t ALERT_LIMIT_VALUE_C1 = 2;
const mtime_t MAX_FOOD_VALUE_C2 = 10; // C2
const mtime_t ALERT_LIMIT_VALUE_C2 = 2;
const mtime_t MAX_FOOD_VALUE_C3 = 10; // C3
const mtime_t ALERT_LIMIT_VALUE_C3 = 2;


#endif /* CONFIG_H_ */
