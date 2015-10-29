#ifndef STEPPER_MOTOR_H_
#define STEPPER_MOTOR_H_

class StepperMotor {
    public:
        StepperMotor();

        void attach(int in1, int in2, int in3, int in4);    // Constructor that will set the inputs
        void setStepDuration(int duration);    // Function used to set the step duration in ms
        void step(int stepNumber);    // Step a certain number of steps. + for one way and - for the other

    private:
        int duration;    // Step duration in ms
        int inputPins[5];    // The input pin numbers
};

#endif /* STEPPER_MOTOR_H_ */
