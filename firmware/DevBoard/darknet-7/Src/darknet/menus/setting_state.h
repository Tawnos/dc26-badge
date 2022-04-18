/*
 * setting_state.h
 *
 *  Created on: May 29, 2018
 *      Author: dcomes
 */

#ifndef DARKNET_MENUS_SETTING_STATE_H_
#define DARKNET_MENUS_SETTING_STATE_H_

#include "darknet7.h"
#include "darknet7_base_state.h"
#include "../KeyStore.h"
#include "../virtual_key_board.h"

class SettingState: public Darknet7BaseState {
public:

		SettingState() : Darknet7BaseState()
	{
		memset(&AgentName[0], 0, sizeof(AgentName));
		Items[0].id = 0;
		Items[0].text = (const char*)"Set Agent Name";
		Items[1].id = 1;
		Items[1].text = (const char*)"Screen Saver Time";
		Items[1].setShouldScroll();
		Items[2].id = 2;
		Items[2].text = (const char*)"Reset Badge Contacts";
		Items[2].setShouldScroll();
	}
	virtual ~SettingState() = default;
protected:
	virtual cmdc0de::ErrorType onInit();
	virtual cmdc0de::StateBase::ReturnStateContext onRun();
	virtual cmdc0de::ErrorType onShutdown();
private:
	cmdc0de::GUIListData SettingList{ (const char*)"MENU", Items, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, sizeof(Items) / sizeof(Items[0]) };
	cmdc0de::GUIListItemData Items[3];
	char AgentName[ContactStore::AGENT_NAME_LENGTH];
	uint8_t SubState{ 0 };
	uint8_t MiscCounter{ 0 };
	VirtualKeyBoard VKB{};
	VirtualKeyBoard::InputHandleContext IHC{ &AgentName[0], sizeof(AgentName) };
};


#endif /* DARKNET_MENUS_SETTING_STATE_H_ */
