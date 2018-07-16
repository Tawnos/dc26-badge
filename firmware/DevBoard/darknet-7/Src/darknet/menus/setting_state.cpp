/*
 * setting_state.cpp
 *
 *  Created on: May 29, 2018
 *      Author: dcomes
 */

#include "setting_state.h"
#include "../darknet7.h"
#include "../virtual_key_board.h"
#include "../libstm32/display/display_device.h"
#include "menu_state.h"

using cmdc0de::ErrorType;
using cmdc0de::StateBase;
using cmdc0de::RGBColor;

SettingState::SettingState() : Darknet7BaseState(), SettingList((const char *) "MENU", Items, 0, 0,
				128, 160, 0, sizeof(Items) / sizeof(Items[0])), AgentName(), SubState(0), MiscCounter(0), VKB(), IHC(&AgentName[0],sizeof(AgentName)) {

	memset(&AgentName[0], 0, sizeof(AgentName));
	Items[0].id = 0;
	Items[0].text = (const char *) "Set Agent Name";
	Items[1].id = 1;
	Items[1].text = (const char *) "Screen Saver Time";
	Items[1].setShouldScroll();
	Items[2].id = 2;
	Items[2].text = (const char *) "Reset Badge Contacts";
	Items[2].setShouldScroll();
}

SettingState::~SettingState() {

}

ErrorType SettingState::onInit() {
	SubState = 0;
	MiscCounter = 0;
	memset(&AgentName[0], 0, sizeof(AgentName));
	DarkNet7::get().getDisplay().fillScreen(RGBColor::BLACK);
	DarkNet7::get().getGUI().drawList(&SettingList);
	return ErrorType();
}


StateBase::ReturnStateContext SettingState::onRun() {
	StateBase *nextState = this;
	if (0 == SubState) {
		if (DarkNet7::get().getButtonInfo().wereAnyOfTheseButtonsReleased(DarkNet7::ButtonInfo::BUTTON_UP)) {
			if (SettingList.selectedItem == 0) {
				SettingList.selectedItem = sizeof(Items) / sizeof(Items[0]) - 1;
			} else {
				SettingList.selectedItem--;
			}
		} else if (DarkNet7::get().getButtonInfo().wereAnyOfTheseButtonsReleased( DarkNet7::ButtonInfo::BUTTON_DOWN)) {
			if (SettingList.selectedItem
					== (sizeof(Items) / sizeof(Items[0]) - 1)) {
				SettingList.selectedItem = 0;
			} else {
				SettingList.selectedItem++;
			}
		} else if (DarkNet7::get().getButtonInfo().wereAnyOfTheseButtonsReleased(DarkNet7::ButtonInfo::BUTTON_LEFT)) {
			SettingList.selectedItem = 0;
		} else if (DarkNet7::get().getButtonInfo().wereAnyOfTheseButtonsReleased(DarkNet7::ButtonInfo::BUTTON_FIRE1 | DarkNet7::ButtonInfo::BUTTON_MID)) {
			SubState = SettingList.selectedItem + 100;
			DarkNet7::get().getDisplay().fillScreen(RGBColor::BLACK);
			switch (SubState) {
			case 100:
				memset(&AgentName[0], 0, sizeof(AgentName));
				VKB.init(VirtualKeyBoard::STDKB, &IHC, 5, DarkNet7::DISPLAY_WIDTH-5, 100, cmdc0de::RGBColor::WHITE, RGBColor::BLACK, RGBColor::BLUE, '_');
				DarkNet7::get().getDisplay().drawString(0, 10, (const char*) "Current agent name:");
				if (*DarkNet7::get().getContacts().getSettings().getAgentName()	== '\0') {
					DarkNet7::get().getDisplay().drawString(0, 20, (const char *) "NOT SET");
				} else {
					DarkNet7::get().getDisplay().drawString(0, 20, DarkNet7::get().getContacts().getSettings().getAgentName());
				}
				DarkNet7::get().getDisplay().drawString(0, 30, (const char*) "Set agent name:");
				DarkNet7::get().getDisplay().drawString(0, 40, &AgentName[0]);
				break;
			case 101:
				DarkNet7::get().getDisplay().drawString(0, 10, (const char*) "Time until badge\ngoes to sleep:", RGBColor::WHITE, RGBColor::BLACK, 1, true);
				DarkNet7::get().getDisplay().drawString(0, 30, (const char*) "Up to increase, down to decrease", RGBColor::WHITE, RGBColor::BLACK, 1, true);
				DarkNet7::get().getDisplay().drawString(0, 40, (const char*) "Fire completes", RGBColor::WHITE, RGBColor::BLACK, 1, true);
				MiscCounter = DarkNet7::get().getContacts().getSettings().getScreenSaverTime();
				break;
			case 102:
				DarkNet7::get().getDisplay().drawString(0, 10, (const char*) "ERASE ALL\nCONTACTS?");
				DarkNet7::get().getDisplay().drawString(0, 30, (const char*) "Fire1 = YES");
				break;
			}
		}
	} else {
		switch (SubState) {
		case 100:
			VKB.process();
			if (DarkNet7::get().getButtonInfo().wereAnyOfTheseButtonsReleased(DarkNet7::ButtonInfo::BUTTON_MID) && AgentName[0] != '\0' && AgentName[0] != ' ' && AgentName[0] != '_') {
				AgentName[ContactStore::AGENT_NAME_LENGTH - 1] = '\0';
				if (DarkNet7::get().getContacts().getSettings().setAgentname(&AgentName[0])) {
					nextState = DarkNet7::get().getDisplayMessageState(	DarkNet7::get().getDisplayMenuState(), (const char *)"Save Successful", 2000);
				} else {
					nextState = DarkNet7::get().getDisplayMessageState(	DarkNet7::get().getDisplayMenuState(), (const char *)"Save FAILED!",	4000);
				}
			} else {
				DarkNet7::get().getDisplay().drawString(0, 40, &AgentName[0]);
			}
			break;
		case 101: {
				if (DarkNet7::get().getButtonInfo().wereAnyOfTheseButtonsReleased(DarkNet7::ButtonInfo::BUTTON_UP)) {
					MiscCounter++;
				} else if (DarkNet7::get().getButtonInfo().wereAnyOfTheseButtonsReleased(DarkNet7::ButtonInfo::BUTTON_DOWN)) {
					MiscCounter--;
				} else if (DarkNet7::get().getButtonInfo().wereAnyOfTheseButtonsReleased(DarkNet7::ButtonInfo::BUTTON_MID)) {
					if (DarkNet7::get().getContacts().getSettings().setScreenSaverTime(MiscCounter)) {
						nextState = DarkNet7::get().getDisplayMessageState(DarkNet7::get().getDisplayMenuState(), (const char *)"Setting saved", 2000);
					} else {
						nextState = DarkNet7::get().getDisplayMessageState(DarkNet7::get().getDisplayMenuState(), (const char *)"Save FAILED!", 4000);
					}
				}
			}
			break;
		case 102:
			if (DarkNet7::get().getButtonInfo().wereAnyOfTheseButtonsReleased(DarkNet7::ButtonInfo::BUTTON_FIRE1)) {
				DarkNet7::get().getContacts().resetToFactory();
			} else if (DarkNet7::get().getButtonInfo().wasAnyButtonReleased()) {
				nextState = DarkNet7::get().getDisplayMenuState();
			}
			break;
		}
	}
	if (SubState < 100 && DarkNet7::get().getButtonInfo().wasAnyButtonReleased()) {
		DarkNet7::get().getGUI().drawList(&SettingList);
	}
	return ReturnStateContext(nextState);
}

ErrorType SettingState::onShutdown() {
	return ErrorType();
}
