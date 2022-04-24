/*
 * DC26.h
 *
 *  Created on: Dec 3, 2017
 *      Author: cmdc0de
 */

#ifndef DARKNET_DC26_H_
#define DARKNET_DC26_H_

#include <app/app.h>
#include <display/display_device.h>
#include <display/display_st7735.h>
#include <app/display_message_state.h>
#include <display/gui.h>
#include <libstm32/error_type.h>
#include "KeyStore.h"
#include "button_info.h"
#include "mcu_to_mcu.h"

#if !defined VIRTUAL_DEVICE
#include <leds/ws2812.h>
#endif

class MenuState;
class TestState;
class SendMsgState;
class SettingState;
class PairingState;
class AddressState;
class Menu3D;
class GameOfLife;
class CommunicationSettingState;
class BadgeInfoState;
class MCUInfoState;
//class Tamagotchi;
class Health;
class Scan;
class SAO;

static const uint8_t MyAddressInfoSector = 3; //same sector as settings just first thing
static const uint32_t MyAddressInfoOffSet = 0;
static const uint8_t SettingSector = 1;
static const uint32_t SettingOffset = 0;
static const uint8_t StartContactSector = 2;
static const uint8_t EndContactSector = 3;


#define START_LANDSCAPE
#ifdef START_LANDSCAPE
static const uint32_t DISPLAY_WIDTH = 160;
static const uint32_t DISPLAY_HEIGHT = 128;
static const cmdc0de::RotationType START_ROT = cmdc0de::RotationType::LandscapeTopLeft;
#else
static const uint32_t DISPLAY_WIDTH = 128;
static const uint32_t DISPLAY_HEIGHT = 160;
static const cmdc0de::RotationType START_ROT = cmdc0de::RotationType::PortraitTopLeft;
#endif

static const uint32_t DISPLAY_OPT_WRITE_ROWS = DISPLAY_HEIGHT;
static uint16_t DrawBuffer[DISPLAY_WIDTH * DISPLAY_OPT_WRITE_ROWS]; //120 wide, 10 pixels high, 2 bytes per pixel (uint16_t)

class DarkNet7 : public cmdc0de::App {
public:
  static constexpr const char* sYES = "Yes";
  static constexpr const char* sNO = "No";
  static constexpr const char* NO_DATA_FROM_ESP = "No data returned from ESP, try resetting ESP.";
  static constexpr const char* BLE_CONNECT_FAILED = "BLE Connection failed.";
  static constexpr const char* BLE_DISCONNECTING = "BLE Disconnecting.";
  static constexpr const char* BLE_PAIRING_SUCCESS = "Paired.";
  static constexpr const char* BLE_PAIRING_FAILED = "Failed to Pair.";


  static const uint16_t BROADCAST_ADDR = 0xFFFF;
public:
  static DarkNet7* instance;

#if !defined VIRTUAL_DEVICE
  DarkNet7(
    cmdc0de::WS2818 leds,
    cmdc0de::DisplayDevice* displayDevice
  ) : Apa106s(leds)
      //		       my Info, start setting address, start Contact address, end contact address
      MyContacts(MyAddressInfoSector, MyAddressInfoOffSet, SettingSector, SettingOffset, StartContactSector, EndContactSector),
      Display(displayDevice),
      DisplayBuffer(displayDevice, DrawBuffer),
      DMS(), 
      MyGUI(Display),
      MyButtons(),
      SequenceNum(0) {}
#else
  DarkNet7(cmdc0de::DisplayDevice* displayDevice) 
    : //		       my Info, start setting address, start Contact address, end contact address
      MyContacts(MyAddressInfoSector, MyAddressInfoOffSet, SettingSector, SettingOffset, StartContactSector, EndContactSector),
      Display(displayDevice),
      DisplayBuffer{ displayDevice, DrawBuffer },
      DMS(),
      MyGUI(Display),
      SequenceNum(0) {}
#endif

  cmdc0de::DisplayMessageState* getDisplayMessageState(cmdc0de::StateBase* bm, const char* message, uint16_t timeToDisplay);
  MenuState* getDisplayMenuState();
  SendMsgState* getSendMsgState();
  SettingState* getSettingState();
  AddressState* getAddressBookState();
  CommunicationSettingState* getCommunicationSettingState();
  BadgeInfoState* getBadgeInfoState();
  Health* getHealthState();
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

  cmdc0de::DisplayDevice& getDisplay() {
    return *Display;
  }

  const cmdc0de::DisplayDevice& getDisplay() const {
    return *Display;
  }

  ContactStore& getContacts();
  const ContactStore& getContacts() const;
  cmdc0de::GUI& getGUI();
  const cmdc0de::GUI& getGUI() const;
  ButtonInfo& getButtonInfo();
  const ButtonInfo& getButtonInfo() const;
  MCUToMCU& getMcuToMcu();
  const MCUToMCU& getMcuToMcu() const;
  uint32_t nextSeq();
  virtual ~DarkNet7() = default;
protected:
  virtual cmdc0de::ErrorType onInit();
  virtual cmdc0de::ErrorType onRun();

private:
#if !defined VIRTUAL_DEVICE
  cmdc0de::WS2818 Apa106s;
#endif
  ContactStore MyContacts;
  cmdc0de::DisplayDevice* Display;
  cmdc0de::DrawBuffer2D16BitColor16BitPerPixel1Buffer DisplayBuffer;
  cmdc0de::DisplayMessageState DMS;
  cmdc0de::GUI MyGUI;
  ButtonInfo MyButtons{};
  uint32_t SequenceNum;
};


namespace cmdc0de {
  class DisplayMessageState;
}

#endif /* DARKNET_DC26_H_ */
