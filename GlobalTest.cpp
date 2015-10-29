#include <signal.h> // signal()

#include <fcntl.h> // fcntl()
#include <termios.h> // struct termios
#include <unistd.h> // usleep, read, write
#include <term.h> // termcap

#include "polyfills/arduinoPolyfill.h"

#include <inttypes.h> // uint8_t
#include <stdio.h> // printf()
#include <stdlib.h> // size_t

#include "../src/clock.h"
#include "../src/tasks.h"
#include "../src/events.h"

struct termios term;
int termFlags;

int pins[80] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
void pinMode(int pin, bool state) {
	// code does nothing
	return;
}
void digitalWrite(int pin, bool value) {
	if (pin > 0 && pin < 80) {
		// if (pin == PIN_STOCK_INDICATOR_O) {
		// 	if (value) {
		// 		tputs(tgoto(tgetstr("cm", NULL), 5, 8), STDOUT_FILENO, &putchar3);
		// 		write(1, "LED", 3);
		// 	} else {
		// 		tputs(tgoto(tgetstr("cm", NULL), 5, 8), STDOUT_FILENO, &putchar3);
		// 		write(1, "   ", 3);
		// 	}
		// 	return;
		// }
		pins[pin] = value;
	}
}

int putchar3(int arg) {
	char c = arg;
	return ::write(1, &c, 1);
}

int digitalRead(int pin) {
	return pins[pin];
}

void delay(long number) {
	// usleep(number * 1000);
}

unsigned long long thisTime = 0;
unsigned long millis() {
	// itoa_rec(thisTime);
	// write(1, "\n", 1);
	return thisTime;
}

bool checkTimersIntegrity(Tasks tasks) {
	if (tasks.getTaskTime(0) >= 2400 || tasks.getTaskTime(1) >= 2400 || tasks.getTaskTime(2) >= 2400 || !(tasks.getTaskTime(0) < tasks.getTaskTime(1)
		&& tasks.getTaskTime(1) < tasks.getTaskTime(2))) return (false);
	return (true);
}

void itoa_rec(unsigned long long value) {
	char c;

	if (value >= 10) {
		itoa_rec(value / 10);
	}

	c = value % 10 + '0';
	::write(1, &c, 1);
}

char *itoa(int value) {
	char *string;
	int mod = 1;
	int string_len = 1;

	while (value / 10 > mod) {
		mod *= 10;
		string_len++;
	}

	string = (char *) malloc(sizeof(char) * (string_len + 1));
	string[string_len] = 0;

	for (int i = 0; i < string_len; i++) {
		string[i] = value / mod % 10 + '0';
		mod /= 10;
	}

	return (string);
}

void my_handler(int s) {
	write(1, "exiting...\n", 11);
	// resets the terminal as non canonical and displayable
	term.c_lflag = (ICANON | ECHO);
	tcsetattr(0, 0, &term);
	termFlags &= ~(O_NONBLOCK);
	fcntl(STDIN_FILENO, F_SETFL, termFlags);
	exit(0);
}


int main(int argc, char const *argv[]) {
	struct sigaction sigIntHandler;
	char *name_term;
	char buffer[3];

	termFlags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, termFlags | O_NONBLOCK);

	if ((name_term = getenv("TERM")) == NULL)
		return (-1);
	if (tgetent(NULL, name_term) != 1)
		return (-1);
	// remplis la structure termios des possibilités du terminal.
	if (tcgetattr(0, &term) == -1)
		return (-1);

	// sets the terminal as canonical and non displayable
	term.c_lflag &= ~(ICANON);
	term.c_lflag &= ~(ECHO);
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;

	if (tcsetattr(0, 0, &term) == -1)
		return (-1);

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	// sigfillset(&sigIntHandler.sa_mask);
	sigaction(SIGINT, &sigIntHandler, NULL);

	int timer = 0;

	setup();
	while(42) {

		timer++;
		if (timer % 5 == 0) {
			timer = 0;
			thisTime++;
		}

		delay(1);

		// resets the entry input simulator
		pins[PIN_SWITCH_LIGHT_I] = LOW;
		// pins[PIN_DIRECTION_UP_I] = LOW;
		pins[PIN_DIRECTION_DOWN_I] = LOW;
		// pins[PIN_DIRECTION_LEFT_I] = LOW;
		pins[PIN_DIRECTION_RIGHT_I] = LOW;
		pins[PIN_RESET_VALUE_I] = LOW;

		if (read(0, buffer, 3) > 0) {
			if (buffer[0] == 3)
				break;

			if (buffer[0] == 27 && buffer[1] == 91) {
				// UP & DOWN
				if (buffer[2] == 65) {
					// pins[PIN_DIRECTION_UP_I] = HIGH;
					// tputs(tgoto(tgetstr("cm", NULL), 18, 5), STDOUT_FILENO, &putchar3);
					// write(STDOUT_FILENO, " UP  ", 5);
					pins[PIN_DIRECTION_DOWN_I] = HIGH;
				} else if (buffer[2] == 66) {
					// tputs(tgoto(tgetstr("cm", NULL), 18, 5), STDOUT_FILENO, &putchar3);
					// write(STDOUT_FILENO, "DOWN ", 5);
					pins[PIN_DIRECTION_DOWN_I] = HIGH;
				// RIGHT & LEFT
				} else if (buffer[2] == 67) {
					// pins[PIN_DIRECTION_LEFT_I] = HIGH;
					// tputs(tgoto(tgetstr("cm", NULL), 18, 5), STDOUT_FILENO, &putchar3);
					// write(STDOUT_FILENO, "LEFT ", 5);
					pins[PIN_DIRECTION_RIGHT_I] = HIGH;
				} else if (buffer[2] == 68) {
					// tputs(tgoto(tgetstr("cm", NULL), 18, 5), STDOUT_FILENO, &putchar3);
					// write(STDOUT_FILENO, "RIGHT", 5);
					pins[PIN_DIRECTION_RIGHT_I] = HIGH;
				}

			// BACKSPACE
			} else if (buffer[0] == 127) {
				pins[PIN_RESET_VALUE_I] = HIGH;
			// ENTER
			} else if (buffer[0] == 10) {
				pins[PIN_SWITCH_LIGHT_I] = HIGH;
			}
		}

		loop();
	}

	// resets the terminal as non canonical and displayable
	term.c_lflag = (ICANON | ECHO);
	tcsetattr(0, 0, &term);
	termFlags &= ~(O_NONBLOCK);
	fcntl(STDIN_FILENO, F_SETFL, termFlags);

	return (0);
}
