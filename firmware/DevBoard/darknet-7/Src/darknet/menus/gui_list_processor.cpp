/*
 * gui_list_processor.cpp
 *
 *  Created on: Jul 16, 2018
 *      Author: dcomes
 */

#include "gui_list_processor.h"
#include "../libstm32/display/gui.h"
#include "../darknet7.h"

bool GUIListProcessor::process(cmdc0de::GUIListData *pl, uint16_t itemCount) {
	bool bHandled = false;
	if (DarkNet7::instance->getButtonInfo().wereAnyOfTheseButtonsReleased(Button::Up)) {
		bHandled = true;
		do {
			if (pl->selectedItem == 0) {
				pl->selectedItem = itemCount - 1;
			} else {
				pl->selectedItem--;
			}
		} while(!pl->items[pl->selectedItem].text || pl->items[pl->selectedItem].text[0]=='\0');
	} else if (DarkNet7::instance->getButtonInfo().wereAnyOfTheseButtonsReleased(Button::Down)) {
		bHandled = true;
		do {
			if (pl->selectedItem == itemCount - 1) {
				pl->selectedItem = 0;
			} else {
				pl->selectedItem++;
			}
		} while(!pl->items[pl->selectedItem].text || pl->items[pl->selectedItem].text[0]=='\0');
	} else if (DarkNet7::instance->getButtonInfo().wereAnyOfTheseButtonsReleased(Button::Left)) {
		bHandled = true;
		pl->selectedItem = 0;
	}
	return bHandled;
}



