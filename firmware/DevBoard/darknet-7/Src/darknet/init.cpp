/*
 * init.cpp
 *
 *  Created on: Dec 3, 2017
 *      Author: dcomes
 */

#include "init.h"
#include "darknet7.h"

DarkNet7::instance = new DarkNet7(
	new Display(DISPLAY_WIDTH, DISPLAY_HEIGHT, START_ROT),
	ButtonInfo()
);

void init() {
	DarkNet7::instance->init();
}

void runOnce() {
	DarkNet7::instance->run();
}

