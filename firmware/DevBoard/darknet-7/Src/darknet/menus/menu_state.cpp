#include "menu_state.h"
#include "../libstm32/display/display_device.h"
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
#include "../mcu_to_mcu.h"
#include "../messaging/stm_to_esp_generated.h"
#include "../messaging/esp_to_stm_generated.h"

using cmdc0de::ErrorType;
using cmdc0de::StateBase;
using cmdc0de::RGBColor;

MenuState::MenuState() :
		Darknet7BaseState(), MenuList("Main Menu", Items, 0, 0, cmdc0de::DISPLAY_WIDTH,
				cmdc0de::DISPLAY_HEIGHT
				, 0, (sizeof(Items) / sizeof(Items[0])))
{
}

MenuState::~MenuState() {

}


ErrorType MenuState::onInit() {
	Items[0].id = 0;
	if (DarkNet7::instance->getContacts().getSettings().isNameSet()) {
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
	DarkNet7::instance->getDisplay().fillScreen(RGBColor::BLACK);
	DarkNet7::instance->getGUI().drawList(&this->MenuList);
	return ErrorType();
}

cmdc0de::StateBase::ReturnStateContext MenuState::onRun() {
	StateBase *nextState = this;
	if (!GUIListProcessor::process(&MenuList,(sizeof(Items) / sizeof(Items[0]))))
	{
		if (DarkNet7::instance->getButtonInfo().wereAnyOfTheseButtonsReleased(ButtonPress::Fire))
		{
			switch (MenuList.selectedItem)
			{
				case 0:
					nextState = DarkNet7::instance->getSettingState();
					break;
				case 1:
					if (DarkNet7::instance->getContacts().getSettings().getAgentName()[0] != '\0') {
						//nextState = DarkNet7::instance->getPairingState();
					} else {
						nextState = DarkNet7::instance->getDisplayMessageState(DarkNet7::instance->getDisplayMenuState(),
								(const char *) "You must set your agent name first", 3000);
					}
					break;
				case 2:
					nextState = DarkNet7::instance->getAddressBookState();
					break;
				case 4:
					nextState = DarkNet7::instance->getGameOfLifeState();
					break;
				case 5:
					nextState = DarkNet7::instance->getBadgeInfoState();
					break;
				case 7:
					nextState = DarkNet7::instance->getCommunicationSettingState();
					break;
				case 8:
					nextState = DarkNet7::instance->getHealthState();
					break;
#if !defined VIRTUAL_DEVICE
				case 3:
					nextState = DarkNet7::instance->get3DState();
					break;
				case 6:
					nextState = DarkNet7::instance->getMCUInfoState();
					break;
				case 9:
					DarkNet7::instance->getScanState()->setNPCOnly(true);
					nextState = DarkNet7::instance->getScanState();
					break;
				case 10:
					nextState = DarkNet7::instance->getTestState();
					break;
				case 11:
					nextState = DarkNet7::instance->getSAOMenuState();
					break;
#endif
			}
		}
	}

	if (DarkNet7::instance->getButtonInfo().wasAnyButtonReleased()) {
		DarkNet7::instance->getGUI().drawList(&this->MenuList);
	}

	return StateBase::ReturnStateContext(nextState);
}

ErrorType MenuState::onShutdown() {
	//MenuList.selectedItem = 0;
	return ErrorType();
}
