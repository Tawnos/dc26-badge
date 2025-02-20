#include "menu_state.h"
#include "../darknet7.h"
#include "test_state.h"
#include "setting_state.h"
#include "pairing_state.h"
#include "AddressState.h"
#include "GameOfLife.h"
#include "menu3d.h"
#include "communications_settings.h"
#include "badge_info_state.h"
#include "mcu_info.h"
#include "tamagotchi.h"
#include "health.h"
#include "scan.h"
#include "gui_list_processor.h"
#include "sao_menu.h"
#include "darknet7_base_state.h"
#include <libstm32/rgbcolor.h>

using cmdc0de::ErrorType;

using cmdc0de::RGBColor;

ErrorType MenuState::onInit() {
	Items[0].id = 0;
	if (darknet->getContactStore()->getSettings().isNameSet()) {
		Items[0].text = (const char *) "Settings";
	} else {
		Items[0].text = (const char *) "Settings *";
	}
	Items[1].id = 1;
	Items[1].text = (const char *) "Badge Pair";
	Items[2].id = 2;
	Items[2].text = (const char *) "Address Book";
	Items[3].id = 3;
	Items[3].text = (const char *) "3D";
	Items[4].id = 4;
	Items[4].text = (const char *) "Screen Saver";
	Items[5].id = 5;
	Items[5].text = (const char *) "STM Info";
	Items[6].id = 6;
	Items[6].text = (const char *) "ESP Info";
	Items[7].id = 7;
	Items[7].text = (const char *) "Communications Settings";
	Items[8].id = 8;
	Items[8].text = (const char *) "Health";
	Items[9].id = 9;
	Items[9].text = (const char *) "Scan for NPCs";
	Items[10].id = 10;
	Items[10].text = (const char *) "Test Badge";
	Items[11].id = 11;
	Items[11].text = (const char *) "Scan: Shitty Addon Badge";
	darknet->getGUI()->fillScreen(RGBColor::BLACK);
	darknet->getGUI()->drawList(&this->MenuList);
	return ErrorType();
}

Darknet7BaseState*  MenuState::onRun() {
	Darknet7BaseState *nextState = this;
	auto buttonInfo = darknet->getButtonInfo();
	if (!GUIListProcessor::process(buttonInfo, &MenuList,(sizeof(Items) / sizeof(Items[0]))))
	{
		if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Fire))
		{
			switch (MenuList.selectedItem)
			{
				case 0:
					nextState = darknet->getSettingState();
					break;
				case 1:
					if (darknet->getContactStore()->getSettings().getAgentName()[0] != '\0') {
						nextState = darknet->getPairingState();
					} else {
						nextState = darknet->getDisplayMessageState(darknet->getDisplayMenuState(),
								(const char *) "You must set your agent name first", 3000);
					}
					break;
				case 2:
					nextState = darknet->getAddressBookState();
					break;
				case 4:
					nextState = darknet->getGameOfLifeState();
					break;
				case 5:
					nextState = darknet->getBadgeInfoState();
					break;
				case 7:
					nextState = darknet->getCommunicationSettingState();
					break;
				case 8:
					nextState = darknet->getHealthState();
					break;
#if !defined VIRTUAL_DEVICE
				case 3:
					nextState = darknet->get3DState();
					break;
				case 6:
					nextState = darknet->getMCUInfoState();
					break;
				case 9:
					darknet->getScanState()->setNPCOnly(true);
					nextState = darknet->getScanState();
					break;
				case 10:
					nextState = darknet->getTestState();
					break;
				case 11:
					nextState = darknet->getSAOMenuState();
					break;
#endif
			}
		}
	}

	if (buttonInfo->wasAnyButtonReleased()) {
		darknet->getGUI()->drawList(&this->MenuList);
	}

	return nextState;
}

ErrorType MenuState::onShutdown() {
	//MenuList.selectedItem = 0;
	return ErrorType();
}
