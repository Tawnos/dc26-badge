/*
 * setting_state.h
 *
 *  Created on: May 29, 2018
 *      Author: dcomes
 */

#ifndef DARKNET_MENUS_SETTING_STATE_H_
#define DARKNET_MENUS_SETTING_STATE_H_

#include "darknet7_base_state.h"
#include "contact.h"
#include <libstm32/display/gui.h>
#include <error_type.h>
#include <stdint.h>

class SettingState : public Darknet7BaseState
{
public:
   using Darknet7BaseState::Darknet7BaseState;
   virtual ~SettingState() = default;
protected:
   virtual cmdc0de::ErrorType onInit() override;
   virtual Darknet7BaseState* onRun() override;
   virtual cmdc0de::ErrorType onShutdown() override;
private:
   cmdc0de::GUIListItemData Items[3]{ {0,"Set Agent Name"},{1, "Screen Saver Time"},{2, "Reset Badge Contacts"} };
   cmdc0de::GUIListData SettingList{ "MENU", Items, 0, 0, cmdc0de::DISPLAY_WIDTH, cmdc0de::DISPLAY_HEIGHT, 0, sizeof(Items) / sizeof(Items[0]) };
   char AgentName[AGENT_NAME_LENGTH]{ 0 };
   uint8_t SubState{ 0 };
   uint8_t MiscCounter{ 0 };
   VirtualKeyBoard::InputHandleContext IHC{ &AgentName[0], sizeof(AgentName) };
};


#endif /* DARKNET_MENUS_SETTING_STATE_H_ */
