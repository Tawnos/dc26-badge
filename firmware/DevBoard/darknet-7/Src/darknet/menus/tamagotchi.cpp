/*
 * tamagotchi.cpp
 *
 *  Created on: May 30, 2018
 *      Author: dcomes
 */



#include "tamagotchi.h"
#include "../darknet7.h"

using cmdc0de::RGBColor;
using cmdc0de::ErrorType;



Tamagotchi::Tamagotchi() : Darknet7BaseState()  {

}

Tamagotchi::~Tamagotchi()
{

}

ErrorType Tamagotchi::onInit()
		{
	/*
	memset(&ListBuffer[0], 0, sizeof(ListBuffer));
	sprintf(&ListBuffer[0][0], "Name: %s", DarkNet7::instance->getContacts()->getSettings().getAgentName());
	sprintf(&ListBuffer[1][0], "Num contacts: %u", DarkNet7::instance->getContacts()->getSettings().getNumContacts());
	sprintf(&ListBuffer[2][0], "REG: %s", getRegCode(DarkNet7::instance->getContacts()));
	sprintf(&ListBuffer[3][0], "UID: %u", DarkNet7::instance->getContacts()->getMyInfo().getUniqueID());
	uint8_t *pCP = DarkNet7::instance->getContacts()->getMyInfo().getCompressedPublicKey();
	sprintf(&ListBuffer[4][0],
			"PK: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
			pCP[0], pCP[1], pCP[2], pCP[3], pCP[4], pCP[5], pCP[6], pCP[7], pCP[8], pCP[9], pCP[10], pCP[11],
			pCP[12],
			pCP[13], pCP[14], pCP[15], pCP[16], pCP[17], pCP[18], pCP[19], pCP[20], pCP[21], pCP[22], pCP[23],
			pCP[24]);
	sprintf(&ListBuffer[5][0], "DEVID: %lu", HAL_GetDEVID());
	sprintf(&ListBuffer[6][0], "REVID: %lu", HAL_GetREVID());
	sprintf(&ListBuffer[7][0], "HAL Version: %lu", HAL_GetHalVersion());
	sprintf(&ListBuffer[8][0], "SVer: %s", VERSION);

	for (uint32_t i = 0; i < (sizeof(Items) / sizeof(Items[0])); i++) {
		Items[i].text = &ListBuffer[i][0];
		Items[i].id = i;
		Items[i].setShouldScroll();
	}
	DarkNet7::instance->getDisplay()->fillScreen(RGBColor::BLACK);
	DarkNet7::instance->getGUI()->drawList(&BadgeInfoList);
	*/
	return ErrorType();

}

Darknet7BaseState*  Tamagotchi::onRun() {

	Darknet7BaseState* nextState = this;
	/*
	uint8_t key = rc.getKB().getLastKeyReleased();
	switch (key) {
		case QKeyboard::UP: {
			if (BadgeInfoList.selectedItem == 0) {
				BadgeInfoList.selectedItem = sizeof(Items) / sizeof(Items[0]) - 1;
			} else {
				BadgeInfoList.selectedItem--;
			}
			break;
		}
		case QKeyboard::DOWN: {
			if (BadgeInfoList.selectedItem == (sizeof(Items) / sizeof(Items[0]) - 1)) {
				BadgeInfoList.selectedItem = 0;
			} else {
				BadgeInfoList.selectedItem++;
			}
			break;
		}
		case QKeyboard::ENTER:
			case QKeyboard::BACK:
			nextState = StateFactory::getMenuState();
			break;
	}
	if (rc.getKB().wasKeyReleased()
			|| (Items[BadgeInfoList.selectedItem].shouldScroll() && getTimesRunCalledSinceLastReset() % 5 == 0)) {
		rc.getGUI()->drawList(&BadgeInfoList);
	}
	*/
	return Darknet7BaseState* (nextState);
}

ErrorType Tamagotchi::onShutdown()
{
	return ErrorType();
}






