/*
 * badge_info_state.cpp
 *
 *  Created on: May 29, 2018
 *      Author: dcomes
 */

#include "badge_info_state.h"
#include "../darknet7.h"
#include "../libstm32/crypto/sha256.h"
#include "menu_state.h"
#include "gui_list_processor.h"

using cmdc0de::RGBColor;
using cmdc0de::ErrorType;


const char *BadgeInfoState::getRegCode(ContactStore *cs) {
	if (RegCode[0] == 0) {
		ShaOBJ hashObj;
		sha256_init(&hashObj);
		sha256_add(&hashObj, cs->getMyInfo().getPrivateKey(),
				PRIVATE_KEY_LENGTH);
		uint16_t id = cs->getMyInfo().getUniqueID();
		sha256_add(&hashObj, (uint8_t *) &id, sizeof(id));
		uint8_t rH[SHA256_HASH_SIZE];
		sha256_digest(&hashObj, &rH[0]);
		sprintf(&RegCode[0], "%02x%02x%02x%02x%02x%02x%02x%02x", rH[0], rH[1],
				rH[2], rH[3], rH[4], rH[5], rH[6], rH[7]);
	}
	return &RegCode[0];
}

static const char *VERSION = "dn7.dc26.1";

ErrorType BadgeInfoState::onInit() {
	memset(&ListBuffer[0], 0, sizeof(ListBuffer));
	sprintf(&ListBuffer[0][0], "Name: %s",
			darknet->getContacts()->getSettings().getAgentName());
	sprintf(&ListBuffer[1][0], "Num contacts: %u",
			darknet->getContacts()->getSettings().getNumContacts());
	sprintf(&ListBuffer[2][0], "REG: %s",
			getRegCode(darknet->getContacts()));
	sprintf(&ListBuffer[3][0], "UID: %u",
			darknet->getContacts()->getMyInfo().getUniqueID());
	uint8_t *pCP =	darknet->getContacts()->getMyInfo().getCompressedPublicKey();
	sprintf(&ListBuffer[4][0], "PK: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
			pCP[0], pCP[1], pCP[2], pCP[3], pCP[4], pCP[5], pCP[6], pCP[7],
			pCP[8], pCP[9], pCP[10], pCP[11], pCP[12], pCP[13], pCP[14],
			pCP[15], pCP[16], pCP[17], pCP[18], pCP[19], pCP[20], pCP[21],
			pCP[22], pCP[23], pCP[24]);
#if !defined VIRTUAL_DEVICE
	sprintf(&ListBuffer[5][0], "DEVID: %lu", HAL_GetDEVID());
	sprintf(&ListBuffer[6][0], "REVID: %lu", HAL_GetREVID());
	sprintf(&ListBuffer[7][0], "HAL Version: %lu", HAL_GetHalVersion());
	sprintf(&ListBuffer[8][0], "SVer: %s", VERSION);
#endif

	for (uint32_t i = 0; i < (sizeof(Items) / sizeof(Items[0])); i++) {
		Items[i].text = &ListBuffer[i][0];
		Items[i].id = i;
		Items[i].setShouldScroll();
	}
	darknet->getGUI()->fillScreen(RGBColor::BLACK);
	darknet->getGUI()->drawList(&BadgeInfoList);
	return ErrorType();
}

Darknet7BaseState*  BadgeInfoState::onRun() {

	Darknet7BaseState *nextState = this;
	if(!GUIListProcessor::process(darknet->getButtonInfo(), &BadgeInfoList,BadgeInfoList.ItemsCount)) {
		if (darknet->getButtonInfo()->wereAnyOfTheseButtonsReleased(ButtonPress::Mid)) {
			nextState = darknet->getDisplayMenuState();
		}
	}
	return nextState;
}

ErrorType BadgeInfoState::onShutdown() {
	return ErrorType();
}

