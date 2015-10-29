#ifndef NAVIGATION_H_
#define NAVIGATION_H_

#include <inttypes.h> // uint8_t

// let's define the structure of the simili matrix
// we put the definition outside the scope so it can be used by outsiders
typedef enum enumNavigationDirection {
	UP = 1,
	DOWN = 2,
	LEFT = 3,
	RIGHT = 4,
} navDirection;

class Navigation {
	// let's define the structure of the simili matrix
	typedef struct structMatrixNode {
		uint8_t id;
		uint8_t x;
		uint8_t y;
		structMatrixNode *up;
		structMatrixNode *down;
		structMatrixNode *left;
		structMatrixNode *right;
	} matrixNode;

	public:
		// constructor will process construction
		Navigation(void);
		// destructor is trivial since we need to keep our
		// navigation simili matrix the whole time the arduino is up
		// ~Navigation();

		// return the reference of the cursor
		matrixNode *getCursor();

		// using uint8_t since there's only 4 directions
		// returns true or false depending on the success of the operation
		bool navigate(navDirection d);

		// set the cursor at the start position
		void resetCursorPosition();

	private:
		// we save our cursor as private so it can't be altered outside
		matrixNode *startCursor;
		matrixNode *cursor;

		// private function to create a new node
		// returns the created node
		matrixNode *createNode(uint8_t id, uint8_t x, uint8_t y);
};

#endif /* NAVIGATION_H_ */
