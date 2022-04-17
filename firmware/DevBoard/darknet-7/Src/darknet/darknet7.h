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
#include "KeyStore.h"
#include "button_info.h"

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

class DarkNet7 : public cmdc0de::App {
public:
  static const char* sYES;
  static const char* sNO;
  static const char* NO_DATA_FROM_ESP;
  static const char* BLE_CONNECT_FAILED;
  static const char* BLE_DISCONNECTING;
  static const char* BLE_PAIRING_SUCCESS;
  static const char* BLE_PAIRING_FAILED;

#define START_LANDSCAPE
#ifdef START_LANDSCAPE
  static const uint32_t DISPLAY_WIDTH = 160;
  static const uint32_t DISPLAY_HEIGHT = 128;
#define START_ROT Rotation::LandscapeTopLeft
#else
  static const uint32_t DISPLAY_WIDTH = 128;
  static const uint32_t DISPLAY_HEIGHT = 160;
#define START_ROT Rotation::PortraitTopLeft
#endif
  static const uint16_t BROADCAST_ADDR = 0xFFFF;
public:
  static DarkNet7* instance;
  DarkNet7(
#if !defined VIRTUAL_DEVICE
    cmdc0de::WS2818 leds,
#endif
  cmdc0de::DisplayDevice* displayDevice,
  cmdc0de::DrawBuffer2D16BitColor16BitPerPixel1Buffer displayBuffer,
  ButtonInfo buttons) :
#if !defined VIRTUAL_DEVICE
    Apa106s(leds)
#endif
    //		       my Info, start setting address, start Contact address, end contact address
    MyContacts(MyAddressInfoSector, MyAddressInfoOffSet, SettingSector, SettingOffset, StartContactSector, EndContactSector),
    Display(displayDevice),
    DisplayBuffer(displayBuffer),
    DMS(), 
    MyGUI(Display),
    MyButtons(buttons),
    SequenceNum(0) {}

  cmdc0de::DisplayMessageState* getDisplayMessageState(cmdc0de::StateBase* bm, const char* message, uint16_t timeToDisplay);
  MenuState* getDisplayMenuState();
  TestState* getTestState();
  SendMsgState* getSendMsgState();
  SettingState* getSettingState();
  PairingState* getPairingState();
  AddressState* getAddressBookState();
  Menu3D* get3DState();
  GameOfLife* getGameOfLifeState();
  CommunicationSettingState* getCommunicationSettingState();
  BadgeInfoState* getBadgeInfoState();
  MCUInfoState* getMCUInfoState();
  //Tamagotchi *getTamagotchiState();
  Health* getHealthState();
  Scan* getScanState();
  SAO* getSAOMenuState();
  cmdc0de::DisplayDevice& getDisplay();
  const cmdc0de::DisplayDevice& getDisplay() const;
  ContactStore& getContacts();
  const ContactStore& getContacts() const;
  cmdc0de::GUI& getGUI();
  const cmdc0de::GUI& getGUI() const;
  ButtonInfo& getButtonInfo();
  const ButtonInfo& getButtonInfo() const;
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
  ButtonInfo MyButtons;
  uint32_t SequenceNum;
};


const char* DarkNet7::sYES = "Yes";
const char* DarkNet7::sNO = "No";
const char* DarkNet7::NO_DATA_FROM_ESP = "No data returned from ESP, try resetting ESP.";
const char* DarkNet7::BLE_CONNECT_FAILED = "BLE Connection failed.";
const char* DarkNet7::BLE_DISCONNECTING = "BLE Disconnecting.";
const char* DarkNet7::BLE_PAIRING_SUCCESS = "Paired.";
const char* DarkNet7::BLE_PAIRING_FAILED = "Failed to Pair.";


namespace cmdc0de {
  class DisplayMessageState;
}

#endif /* DARKNET_DC26_H_ */
