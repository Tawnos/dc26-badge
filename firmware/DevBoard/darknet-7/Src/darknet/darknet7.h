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
#if !defined VIRTUAL_DEVICE
#include <leds/ws2812.h>
#include <display/display_st7735.h>
#endif

#include <libstm32/rgbcolor.h>
#include <libstm32/app/app.h>
#include <libstm32/app/display_message_state.h>
#include <display/framebuf.h>
#include <display/DrawBuffer2D16BitPerPixel1Buffer.h>
#include <display/display_device.h>
#include <display/gui.h>
#include <libstm32/error_type.h>

#include "settings_info.h"
#include "contact_store.h"
#include "contact.h"
#include "button_info.h"
#include "mcu_to_mcu.h"

#include "menus/darknet7_base_state.h"
#include "menus/menu_state.h"
#include "menus/setting_state.h"
#include "menus/pairing_state.h"
#include "menus/AddressState.h"
#include "menus/GameOfLife.h"
#include "menus/badge_info_state.h"
#include "menus/SendMsgState.h"
#include "menus/communications_settings.h"
#include "menus/health.h"

class DarkNet7 : public cmdc0de::App
{
public:
#if !defined VIRTUAL_DEVICE
   DarkNet7(
      cmdc0de::WS2818 leds,
      cmdc0de::DisplayDevice* displayDevice,
      ContactStore* contactStore = new ContactStore{ this, cmdc0de::MyAddressInfoSector, cmdc0de::MyAddressInfoOffSet, cmdc0de::SettingSector, cmdc0de::SettingOffset, cmdc0de::StartContactSector, cmdc0de::EndContactSector })
      : Apa106s(leds)
      //		       my Info, start setting address, start Contact address, end contact address
      contactStore(contactStore),
      Display(displayDevice),
      DisplayBuffer(displayDevice, DrawBuffer),
      MyGUI(Display)
   {}
#else
   DarkNet7(
      uint8_t* SettingsMemoryStart,
      uint8_t* SettingsMemoryEnd,
      cmdc0de::DisplayDevice* displayDevice) 
      : Display(displayDevice),
         DisplayBuffer(displayDevice->getFrameBuffer()),
         gui{ DisplayBuffer, &Font_5x7 },
      contactStore(new ContactStore( mcu, SettingsMemoryStart, SettingsMemoryEnd )){ }
#endif
   virtual ~DarkNet7()
   {
      delete contactStore;
   };

   cmdc0de::DisplayMessageState* getDisplayMessageState(Darknet7BaseState* bm, const char* message, uint16_t timeToDisplay)
   {
      DMS->setMessage(message);
      DMS->setNextState(bm);
      DMS->setTimeInState(timeToDisplay);
      return DMS;
   }
   MenuState* getDisplayMenuState() { return MyMenu; }
   SendMsgState* getSendMsgState() { return MySendMsgState; }
   SettingState* getSettingState() { return MySettingState; }
   AddressState* getAddressBookState() { return MyAddressState; }
   CommunicationSettingState* getCommunicationSettingState() { return MyCommunicationSettings; }
   BadgeInfoState* getBadgeInfoState() { return MyBadgeInfoState; }
   Health* getHealthState() { return MyHealth; }
   GameOfLife* getGameOfLifeState() { return MyGameOfLifeState; }
   PairingState* getPairingState() { return pairingState; }

#if !defined VIRTUAL_DEVICE
   MCUInfoState* getMCUInfoState();
   TestState* getTestState();
   Menu3D* get3DState();
   //Tamagotchi *getTamagotchiState();
   Scan* getScanState();
   SAO* getSAOMenuState();
#endif

   cmdc0de::DisplayDevice* getDisplay() { return Display; }
   const cmdc0de::DisplayDevice* getDisplay() const { return Display; }

   ContactStore* getContactStore() { return contactStore; }
   const ContactStore* getContactStore() const { return contactStore; }

   cmdc0de::GUI* getGUI() { return &gui; }
   const cmdc0de::GUI* getGUI() const { return &gui; }

   ButtonInfo* getButtonInfo() { return &MyButtons; }
   const ButtonInfo* getButtonInfo() const { return &MyButtons; }

   uint32_t nextSeq() { return ++SequenceNum; }

   MCUToMCU* getMcuToMcu() { return mcu; }
   const MCUToMCU* getMcuToMcu() const { return mcu; }

protected:
   virtual cmdc0de::ErrorType onInit() override;
   virtual cmdc0de::ErrorType onRun() override;

private:
#if !defined VIRTUAL_DEVICE
   cmdc0de::WS2818 Apa106s;
#endif
   ContactStore* contactStore{ nullptr };
   cmdc0de::DisplayDevice* Display;
   cmdc0de::FrameBuf* DisplayBuffer;
   cmdc0de::GUI gui;
   ButtonInfo MyButtons{ };
   uint32_t SequenceNum{ 0 };

   cmdc0de::DisplayMessageState* DMS = new cmdc0de::DisplayMessageState(this);
   MenuState* MyMenu = new MenuState(this);
   SendMsgState* MySendMsgState = new SendMsgState(this);
   SettingState* MySettingState = new SettingState(this);
   AddressState* MyAddressState = new AddressState(this);
   PairingState* pairingState = new PairingState(this); 
   CommunicationSettingState* MyCommunicationSettings = new CommunicationSettingState(this);
   BadgeInfoState* MyBadgeInfoState = new BadgeInfoState(this);
   Health* MyHealth = new Health(this);
   GameOfLife* MyGameOfLifeState = new GameOfLife(this);
   MCUToMCU* mcu = new MCUToMCU{};
};



namespace cmdc0de {
   class DisplayMessageState;
}

#endif /* DARKNET_DC26_H_ */
