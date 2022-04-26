/*
 * DC26.h
 *
 *  Created on: Dec 3, 2017
 *      Author: cmdc0de
 */

#pragma once
#ifndef DARKNET_DC26_H_
#define DARKNET_DC26_H_
#include "common.h"
#include <app/app.h>
#include <display/display_device.h>
#include <display/display_st7735.h>
#include <app/display_message_state.h>
#include <display/gui.h>
#include <libstm32/error_type.h>
#if !defined VIRTUAL_DEVICE
#include <leds/ws2812.h>
#endif

#include "contact_store.h"
#include "button_info.h"
#include "mcu_to_mcu.h"

#include "menus/menu_state.h"
#include "menus/setting_state.h"
#include "menus/pairing_state.h"
#include "menus/AddressState.h"
#include "menus/badge_info_state.h"
#include "menus/SendMsgState.h"
#include "menus/communications_settings.h"
#include "menus/health.h"

class DarkNet7 : public cmdc0de::App {
public:

public:
  static DarkNet7* instance;

#if !defined VIRTUAL_DEVICE
  DarkNet7(
    cmdc0de::WS2818 leds,
    cmdc0de::DisplayDevice* displayDevice,
    ContactStore contactStore = ContactStore{  }
  ) : Apa106s(leds)
    //		       my Info, start setting address, start Contact address, end contact address
    MyContacts(contactStore),
    Display(displayDevice),
    DisplayBuffer(displayDevice, DrawBuffer),
    MyGUI(Display){}
#else
  DarkNet7(
    cmdc0de::DisplayDevice* displayDevice,
    ContactStore* contactStore
  ) : Display(displayDevice),
      DisplayBuffer{ displayDevice, cmdc0de::DrawBuffer },
      MyGUI(Display),
      MyContacts(contactStore){}
#endif
  virtual ~DarkNet7() = default;

  cmdc0de::DisplayMessageState* getDisplayMessageState(cmdc0de::StateBase* bm, const char* message, uint16_t timeToDisplay) 
  {
    DMS.setMessage(message);
    DMS.setNextState(bm);
    DMS.setTimeInState(timeToDisplay);
    DMS.setDisplay(Display);
    return &DMS;
  }
  MenuState* getDisplayMenuState() { return &MyMenu; }
  SendMsgState* getSendMsgState() { return &MySendMsgState; }
  SettingState* getSettingState() { return &MySettingState; }
  AddressState* getAddressBookState() { return &MyAddressState; }
  CommunicationSettingState* getCommunicationSettingState() { return &MyCommunicationSettings; }
  BadgeInfoState* getBadgeInfoState() { return &MyBadgeInfoState; }
  Health* getHealthState() { return &MyHealth; }
#if !defined VIRTUAL_DEVICE
  MCUInfoState* getMCUInfoState();
  TestState* getTestState();
  Menu3D* get3DState();
  GameOfLife* getGameOfLifeState();
  //Tamagotchi *getTamagotchiState();
  Scan* getScanState();
  PairingState* getPairingState();
  SAO* getSAOMenuState();
#endif

  cmdc0de::DisplayDevice& getDisplay() { return *Display; }
  const cmdc0de::DisplayDevice& getDisplay() const { return *Display; }

  ContactStore& getContacts() { return *MyContacts; }
  const ContactStore& getContacts() const { return *MyContacts; }

  cmdc0de::GUI& getGUI() { return MyGUI; }
  const cmdc0de::GUI& getGUI() const { return MyGUI; }

  ButtonInfo& getButtonInfo() { return MyButtons; }
  const ButtonInfo& getButtonInfo() const { return MyButtons; }

  uint32_t nextSeq() { return ++SequenceNum; }

  MCUToMCU& getMcuToMcu();
  const MCUToMCU& getMcuToMcu() const;
protected:
  virtual cmdc0de::ErrorType onInit();
  virtual cmdc0de::ErrorType onRun();

private:
#if !defined VIRTUAL_DEVICE
  cmdc0de::WS2818 Apa106s;
#endif
  ContactStore* MyContacts = new ContactStore(cmdc0de::MyAddressInfoSector, cmdc0de::MyAddressInfoOffSet, cmdc0de::SettingSector, cmdc0de::SettingOffset, cmdc0de::StartContactSector, cmdc0de::EndContactSector);
  cmdc0de::DisplayDevice* Display;
  cmdc0de::DrawBuffer2D16BitColor16BitPerPixel1Buffer DisplayBuffer;
  cmdc0de::GUI MyGUI;
  ButtonInfo MyButtons{};
  uint32_t SequenceNum{ 0 };

  MenuState MyMenu{};
  cmdc0de::DisplayMessageState DMS{ };
  SendMsgState MySendMsgState{};
  SettingState MySettingState{};
  AddressState MyAddressState{};
  CommunicationSettingState MyCommunicationSettings{};
  BadgeInfoState MyBadgeInfoState{};
  Health MyHealth{};

};



namespace cmdc0de {
  class DisplayMessageState;
}

#endif /* DARKNET_DC26_H_ */
