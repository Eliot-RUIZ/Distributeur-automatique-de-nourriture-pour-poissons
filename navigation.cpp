/*
* @Author: Rémy Ruiz
* @Date:   2015-08-01 10:49:36
* @Last Modified by:   Rémy Ruiz
* @Last Modified time: 2015-08-01 10:58:57
*/
#include "navigation.h"

#include <stddef.h> // NULL
// #include <stdlib.h> // malloc(), NULL

#ifdef PROD
	#include <avr/pgmspace.h> // PROGMEM -> unused at the time
#endif

Navigation::Navigation() {
	// temporary cursor to register previous nodes
	matrixNode *temporaryCursor;

	// primary node saved as it is for interface reset purposes
	// horloge menu node
	this->cursor = this->startCursor = createNode(0, 0, 2);

	// decimal hours
	temporaryCursor = createNode(1, 4, 3);
	this->startCursor->right = temporaryCursor;
	temporaryCursor->left = this->startCursor;

	// hours
	this->cursor = createNode(2, 5, 3);
	temporaryCursor->right = this->cursor;
	this->cursor->left = temporaryCursor;

	// decimal minutes
	temporaryCursor = createNode(3, 7, 3);
	temporaryCursor->left = this->cursor;
	this->cursor->right = temporaryCursor;

	// minutes
	this->cursor = createNode(4, 8, 3);
	temporaryCursor->right = this->cursor;
	this->cursor->left = temporaryCursor;
	this->cursor->right = this->startCursor; // makes the nodes
	this->startCursor->left = this->cursor; // refer to each other

	// feeding scheddule menu
	this->cursor = createNode(5, 0, 3);
	this->startCursor->down = this->cursor; // makes the nodes
	this->cursor->up = this->startCursor; // refer to each other
	this->cursor->down = this->startCursor; // makes the nodes
	this->startCursor->up = this->cursor; // refer to each other

	// compartiement1 feeding scheddule selection
	temporaryCursor = createNode(6, 3, 3);
	this->cursor->right = temporaryCursor;
	temporaryCursor->left = this->cursor;

	// decimal hours
	this->cursor = createNode(7, 7, 3);
	temporaryCursor->right = this->cursor;
	this->cursor->left = temporaryCursor;

	// hours
	temporaryCursor = createNode(8, 8, 3);
	this->cursor->right = temporaryCursor;
	temporaryCursor->left = this->cursor;

	// decimal minutes
	this->cursor = createNode(9, 10, 3);
	temporaryCursor->right = this->cursor;
	this->cursor->left = temporaryCursor;

	// minutes
	temporaryCursor = createNode(10, 11, 3);
	this->cursor->right = temporaryCursor;
	temporaryCursor->left = this->cursor;
	// tricky part
	temporaryCursor->right = this->startCursor->down; // makes the nodes
	this->startCursor->down->left = temporaryCursor; // refer to each other

	// compartiement2 feeding scheddule selection
	temporaryCursor = createNode(11, 3, 4);
	temporaryCursor->left = this->startCursor->down; // return to root
	// tricky part
	temporaryCursor->up = this->startCursor->down->right; // links the nodes
	this->startCursor->down->right->down = temporaryCursor; // vertically

	// decimal hours
	this->cursor = createNode(12, 7, 4);
	temporaryCursor->right = this->cursor;
	this->cursor->left = temporaryCursor;

	// hours
	temporaryCursor = createNode(13, 8, 4);
	this->cursor->right = temporaryCursor;
	temporaryCursor->left = this->cursor;

	// decimal minutes
	this->cursor = createNode(14, 10, 4);
	temporaryCursor->right = this->cursor;
	this->cursor->left = temporaryCursor;

	// minutes
	temporaryCursor = createNode(15, 11, 4);
	this->cursor->right = temporaryCursor;
	temporaryCursor->left = this->cursor;
	// tricky part
	temporaryCursor->right = this->startCursor->down; // return to root

	// compartiement3 feeding scheddule selection
	temporaryCursor = createNode(16, 3, 5);
	temporaryCursor->left = this->startCursor->down; // return to root
	// tricky part
	temporaryCursor->up = this->startCursor->down->right->down; // links the nodes
	this->startCursor->down->right->down->down = temporaryCursor; // vertically
	this->startCursor->down->right->up = temporaryCursor; // makes the nodes
	temporaryCursor->down = this->startCursor->down->right; // refer to each other

	// decimal hours
	this->cursor = createNode(17, 7, 5);
	temporaryCursor->right = this->cursor;
	this->cursor->left = temporaryCursor;

	// hours
	temporaryCursor = createNode(18, 8, 5);
	this->cursor->right = temporaryCursor;
	temporaryCursor->left = this->cursor;

	// decimal minutes
	this->cursor = createNode(19, 10, 5);
	temporaryCursor->right = this->cursor;
	this->cursor->left = temporaryCursor;

	// minutes
	temporaryCursor = createNode(20, 11, 5);
	this->cursor->right = temporaryCursor;
	temporaryCursor->left = this->cursor;
	// tricky part
	temporaryCursor->right = this->startCursor->down; // return to root

	// reset the cursor current position
	this->cursor = this->startCursor;
}

Navigation::matrixNode *Navigation::getCursor() {
	return (this->cursor);
}

Navigation::matrixNode *Navigation::createNode(uint8_t id, uint8_t x, uint8_t y) {
	// TODO: check for avr/pgmspace instead of classic malloc
	// matrixNode *cursor = (matrixNode *) malloc(sizeof(*cursor));
	matrixNode *cursor = new matrixNode();
	cursor->id = id;
	cursor->x = x;
	cursor->y = y;
	cursor->up = NULL;
	cursor->down = NULL;
	cursor->right = NULL;
	cursor->left = NULL;
	return (cursor);
}

// TODO: mettre à jour l'écran lcd
bool Navigation::navigate(navDirection d) {
	if (d == UP
		&& this->cursor->up != NULL) {
		this->cursor = this->cursor->up;
		return (true);
	} else if (d == DOWN
		&& this->cursor->down != NULL) {
		this->cursor = this->cursor->down;
		return (true);
	} else if (d == LEFT
		&& this->cursor->left != NULL) {
		this->cursor = this->cursor->left;
		return (true);
	} else if (d == RIGHT
		&& this->cursor->right != NULL) {
		this->cursor = this->cursor->right;
		return (true);
	}

	// an error seems to have happened
	return (false);
}

void Navigation::resetCursorPosition() {
	this->cursor = this->startCursor;
}
