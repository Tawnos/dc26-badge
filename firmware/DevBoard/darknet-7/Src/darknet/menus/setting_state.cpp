/*
 * setting_state.cpp
 *
 *  Created on: May 29, 2018
 *      Author: dcomes
 */

#include "setting_state.h"
#include "../darknet7.h"
#include "../virtual_key_board.h"
#include <libstm32/display/display_device.h>
#include "menu_state.h"
#include "../darknet7.h"
#if !defined VIRTUAL_DEVICE
#include "../messaging/stm_to_esp_generated.h"
#include "../messaging/esp_to_stm_generated.h"
#endif

using cmdc0de::ErrorType;
using cmdc0de::StateBase;
using cmdc0de::RGBColor;

ErrorType SettingState::onInit() {
  SubState = 0;
  MiscCounter = 0;
  memset(&AgentName[0], 0, sizeof(AgentName));
  DarkNet7::instance->getDisplay().fillScreen(RGBColor::BLACK);
  DarkNet7::instance->getGUI().drawList(&SettingList);
  IHC.set(&AgentName[0], sizeof(AgentName));
  return ErrorType();
}

static char Misc[8] = { '\0' };

StateBase::ReturnStateContext SettingState::onRun() {
  StateBase* nextState = this;
  auto buttonInfo = DarkNet7::instance->getButtonInfo();
  if (0 == SubState) {
    if (buttonInfo.wereAnyOfTheseButtonsReleased(Button::Up)) {
      if (SettingList.selectedItem == 0) {
        SettingList.selectedItem = sizeof(Items) / sizeof(Items[0]) - 1;
      }
      else {
        SettingList.selectedItem--;
      }
    }
    else if (buttonInfo.wereAnyOfTheseButtonsReleased(Button::Down)) {
      if (SettingList.selectedItem
        == (sizeof(Items) / sizeof(Items[0]) - 1)) {
        SettingList.selectedItem = 0;
      }
      else {
        SettingList.selectedItem++;
      }
    }
    else if (buttonInfo.wereAnyOfTheseButtonsReleased(Button::Left)) {
      SettingList.selectedItem = 0;
    }
    else if (buttonInfo.wereAnyOfTheseButtonsReleased(Button::Fire) || buttonInfo.wereAnyOfTheseButtonsReleased(Button::Mid)) {
      SubState = SettingList.selectedItem + 100;
      DarkNet7::instance->getDisplay().fillScreen(RGBColor::BLACK);
      switch (SubState) {
      case 100:
        memset(&AgentName[0], 0, sizeof(AgentName));
        VKB.init(VirtualKeyBoard::STDKBNames, &IHC, 5, DISPLAY_WIDTH - 5, 80, cmdc0de::RGBColor::WHITE, RGBColor::BLACK, RGBColor::BLUE, '_');
        DarkNet7::instance->getDisplay().drawString(0, 10, (const char*)"Current agent name:");
        if (*DarkNet7::instance->getContacts().getSettings().getAgentName() == '\0') {
          DarkNet7::instance->getDisplay().drawString(0, 20, (const char*)"NOT SET");
        }
        else {
          DarkNet7::instance->getDisplay().drawString(0, 20, DarkNet7::instance->getContacts().getSettings().getAgentName());
        }
        DarkNet7::instance->getDisplay().drawString(0, 40, (const char*)"MID button completes entry");
        DarkNet7::instance->getDisplay().drawString(0, 50, (const char*)"Set agent name:");
        DarkNet7::instance->getDisplay().drawString(0, 60, &AgentName[0]);
        break;
      case 101:
        MiscCounter = DarkNet7::instance->getContacts().getSettings().getScreenSaverTime();
        break;
      case 102:
        DarkNet7::instance->getDisplay().drawString(0, 10, (const char*)"ERASE ALL\nCONTACTS?");
        DarkNet7::instance->getDisplay().drawString(0, 30, (const char*)"Fire1 = YES");
        break;
      }
    }
  }
  else {
    switch (SubState) {
    case 100:
      VKB.process();
      if (buttonInfo.wereAnyOfTheseButtonsReleased(Button::Mid) && AgentName[0] != '\0' && AgentName[0] != ' ' && AgentName[0] != '_') {
        AgentName[ContactStore::AGENT_NAME_LENGTH - 1] = '\0';
        if (DarkNet7::instance->getContacts().getSettings().setAgentname(&AgentName[0])) {
          flatbuffers::FlatBufferBuilder fbb;
          auto r = darknet7::CreateBLESetDeviceNameDirect(fbb, DarkNet7::instance->getContacts().getSettings().getAgentName());
          auto z = darknet7::CreateSTMToESPRequest(fbb, DarkNet7::instance->nextSeq(), darknet7::STMToESPAny_BLESetDeviceName, r.Union());
          darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, z);
          DarkNet7::instance->getMcuToMcu().send(fbb);
          nextState = DarkNet7::instance->getDisplayMessageState(DarkNet7::instance->getDisplayMenuState(), (const char*)"Save Successful", 2000);
        }
        else {
          nextState = DarkNet7::instance->getDisplayMessageState(DarkNet7::instance->getDisplayMenuState(), (const char*)"Save FAILED!", 4000);
        }
      }
      else {
        DarkNet7::instance->getDisplay().drawString(0, 60, &AgentName[0]);
      }
      break;
    case 101: {
      if (MiscCounter > 9) MiscCounter = 9;
      else if (MiscCounter < 1) MiscCounter = 1;
      sprintf(&Misc[0], "%d", (int)MiscCounter);
      DarkNet7::instance->getDisplay().drawString(0, 10, (const char*)"Badge Sleep Time:", RGBColor::WHITE, RGBColor::BLACK, 1, true);
      DarkNet7::instance->getDisplay().drawString(0, 30, (const char*)"Up to increase, down to decrease", RGBColor::WHITE, RGBColor::BLACK, 1, true);
      DarkNet7::instance->getDisplay().drawString(10, 60, &Misc[0], RGBColor::WHITE, RGBColor::BLACK, 1, true);
      DarkNet7::instance->getDisplay().drawString(0, 100, (const char*)"MID Button completes", RGBColor::WHITE, RGBColor::BLACK, 1, true);
      if (buttonInfo.wereAnyOfTheseButtonsReleased(Button::Up)) {
        MiscCounter++;
      }
      else if (buttonInfo.wereAnyOfTheseButtonsReleased(Button::Down)) {
        MiscCounter--;
      }
      else if (buttonInfo.wereAnyOfTheseButtonsReleased(Button::Mid)) {
        if (DarkNet7::instance->getContacts().getSettings().setScreenSaverTime(MiscCounter)) {
          nextState = DarkNet7::instance->getDisplayMessageState(DarkNet7::instance->getDisplayMenuState(), (const char*)"Setting saved", 2000);
        }
        else {
          nextState = DarkNet7::instance->getDisplayMessageState(DarkNet7::instance->getDisplayMenuState(), (const char*)"Save FAILED!", 4000);
        }
      }
    }
            break;
    case 102:
      if (buttonInfo.wereAnyOfTheseButtonsReleased(Button::Fire)) {
        DarkNet7::instance->getContacts().resetToFactory();
        nextState = DarkNet7::instance->getDisplayMenuState();
      }
      else if (buttonInfo.wasAnyButtonReleased()) {
        nextState = DarkNet7::instance->getDisplayMenuState();
      }
      break;
    }
  }
  if (SubState < 100 && buttonInfo.wasAnyButtonReleased()) {
    DarkNet7::instance->getGUI().drawList(&SettingList);
  }
  return ReturnStateContext(nextState);
}

ErrorType SettingState::onShutdown() {
  return ErrorType();
}
