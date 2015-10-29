#!/bin/sh
# @Author: Rémy Ruiz
# @Date:   2015-08-24 00:18:59
# @Last Modified by:   Rémy Ruiz
# @Last Modified time: 2015-09-23 17:25:10

echo "rm bin/* || mkdir bin"
rm bin/* || mkdir bin

echo "g++ tests/navigationTests.cpp src/navigation.cpp && mv a.out bin/navigationTests" \
&& g++ tests/navigationTests.cpp src/navigation.cpp && mv a.out bin/navigationTests \
\
&& echo "g++ tests/clockTests.cpp src/clock.cpp && mv a.out bin/clockTests" \
&& g++ tests/clockTests.cpp src/clock.cpp && mv a.out bin/clockTests \
\
&& echo "g++ tests/tasksTests.cpp src/stepper_motor.cpp src/clock.cpp src/tasks.cpp && mv a.out bin/tasksTests" \
&& g++ tests/tasksTests.cpp src/stepper_motor.cpp src/clock.cpp src/tasks.cpp && mv a.out bin/tasksTests \
\
&& echo "g++ tests/globalTests.cpp src/main.cpp src/stepper_motor.cpp tests/polyfills/lcdPolyfill.cpp src/navigation.cpp src/clock.cpp src/tasks.cpp src/events.cpp -lncurses && mv a.out bin/globalTests" \
&& g++ tests/globalTests.cpp src/main.cpp src/stepper_motor.cpp tests/polyfills/lcdPolyfill.cpp src/navigation.cpp src/clock.cpp src/tasks.cpp src/events.cpp -lncurses && mv a.out bin/globalTests \
\
&& echo "tests" \
&& bin/navigationTests "A" \
&& bin/clockTests "A" \
&& bin/tasksTests "A" \
\
&& echo "done"
