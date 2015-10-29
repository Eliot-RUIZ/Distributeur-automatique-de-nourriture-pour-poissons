#include "config.h"

#ifdef DEV
    // Polyfills for test purpose only
    #include "../tests/polyfills/arduinoPolyfill.h"
    #include <stdlib.h>
#endif
#ifdef PROD
    // Libraries
    #if ARDUINO < 100
        #include <WProgram.h> // delay(), pinMode(), digitalWrite()
    #else
        #include <Arduino.h> // delay(), pinMode(), digitalWrite()
    #endif
#endif

#include "stepper_motor.h"

StepperMotor::StepperMotor() {

}

void StepperMotor::attach(int in1, int in2, int in3, int in4) {
    // Record pin numbers in the inputPins array
    this->inputPins[0] = in1;
    this->inputPins[1] = in2;
    this->inputPins[2] = in3;
    this->inputPins[3] = in4;

    // Iterate through the inputPins array, setting each one to output mode
    for(int inputCount = 0; inputCount < 4; inputCount++) {
        pinMode(this->inputPins[inputCount], OUTPUT);
    }

    this->duration = 1;
}

void StepperMotor::step(int stepNumber) {
    /*
        The following 2D array represents the sequence that must be
        used to acheive rotation. The rows correspond to each step, and
        the columns correspond to each input. L
    */
    bool sequence[9][5] = {{LOW, LOW, LOW, HIGH },
                          {LOW, LOW, HIGH, HIGH},
                          {LOW, LOW, HIGH, LOW },
                          {LOW, HIGH, HIGH, LOW},
                          {LOW, HIGH, LOW, LOW },
                          {HIGH, HIGH, LOW, LOW},
                          {HIGH, LOW, LOW, LOW },
                          {HIGH, LOW, LOW, HIGH}};

    int factor = abs(stepNumber) / stepNumber;    // If stepNumber is +, factor = 1. If stepNumber is -, factor = -1
    stepNumber = abs(stepNumber);    // If stepNumber was in fact negative, make positive for future operations

    /*
        The following algorithm runs through the sequence the specified number
        of times
    */
    for (int sequenceNum = 0;  sequenceNum <= stepNumber / 8; sequenceNum++) {
        for (int position = 0; (position < 8) && (position < (stepNumber - sequenceNum * 8)); position++) {
            delay(duration);
            for (int inputCount = 0; inputCount < 4; inputCount++) {
                digitalWrite(this->inputPins[inputCount], sequence[(int) (3.5 - (3.5 * factor) + (factor * position))][inputCount]);
            }
        }
    }
}
