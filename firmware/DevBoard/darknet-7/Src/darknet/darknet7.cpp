/*
 * DC26.cpp
 *
 *  Created on: Dec 3, 2017
 *      Author: dcomes
 */

#include "darknet7.h"
#include <libstm32/display/display_device.h>
#include <libstm32/display/gui.h>
#include <libstm32/display/fonts.h>
#include <libstm32/config.h>
#include <libstm32/logger.h>
#include <libstm32/app/display_message_state.h>
#include "KeyStore.h"
#include "menus/test_state.h"
#include "menus/SendMsgState.h"
#include "menus/menu_state.h"
#include "menus/setting_state.h"
#include "menus/pairing_state.h"
#include "menus/AddressState.h"
#include "menus/menu3d.h"
#include "menus/GameOfLife.h"
#include "menus/badge_info_state.h"
#include "menus/mcu_info.h"
#include "menus/tamagotchi.h"
#include "menus/communications_settings.h"
#include "messaging/stm_to_esp_generated.h"
#include "mcu_to_mcu.h"
#include "menus/health.h"
#include "menus/scan.h"
#include "menus/sao_menu.h"
#include "art/images.h"

using cmdc0de::ErrorType;
using cmdc0de::DrawBufferNoBuffer;
using cmdc0de::GUIListData;
using cmdc0de::GUIListItemData;
using cmdc0de::GUI;
using cmdc0de::RGBColor;

static const uint32_t DISPLAY_OPT_WRITE_ROWS = DarkNet7::DISPLAY_HEIGHT;
static uint16_t DrawBuffer[DarkNet7::DISPLAY_WIDTH * DISPLAY_OPT_WRITE_ROWS]; //120 wide, 10 pixels high, 2 bytes per pixel (uint16_t)

static MenuState MyMenu;
static TestState MyTestState;
static SendMsgState MySendMsgState;
static SettingState MySettingState;
static PairingState MyPairingState;
static AddressState MyAddressState;
static Menu3D MyMenu3D;
static GameOfLife MyGameOfLife;
static CommunicationSettingState MyCommunicationSettings;
static BadgeInfoState MyBadgeInfoState;
static MCUInfoState MyMCUInfoState;
//static Tamagotchi MyTamagotchi;
static Health MyHealth;
static Scan MyScan;
static SAO MySAO;

cmdc0de::DisplayDevice& DarkNet7::getDisplay() {
  return *Display;
}

const cmdc0de::DisplayDevice& DarkNet7::getDisplay() const {
  return *Display;
}

ContactStore& DarkNet7::getContacts() {
  return MyContacts;
}

const ContactStore& DarkNet7::getContacts() const {
  return MyContacts;
}

cmdc0de::GUI& DarkNet7::getGUI() {
  return MyGUI;
}

const cmdc0de::GUI& DarkNet7::getGUI() const {
  return MyGUI;
}

ButtonInfo& DarkNet7::getButtonInfo() {
  return MyButtons;
}

const ButtonInfo& DarkNet7::getButtonInfo() const {
  return MyButtons;
}

cmdc0de::DisplayMessageState* DarkNet7::getDisplayMessageState(cmdc0de::StateBase* bm, const char* message,
  uint16_t timeToDisplay) {
  DMS.setMessage(message);
  DMS.setNextState(bm);
  DMS.setTimeInState(timeToDisplay);
  DMS.setDisplay(Display);
  return &DMS;
}

uint32_t DarkNet7::nextSeq() {
  return ++SequenceNum;
}

SAO* DarkNet7::getSAOMenuState() {
  return &MySAO;
}


MenuState* DarkNet7::getDisplayMenuState() {
  return &MyMenu;
}

TestState* DarkNet7::getTestState() {
  return &MyTestState;
}

SendMsgState* DarkNet7::getSendMsgState() {
  return &MySendMsgState;
}

SettingState* DarkNet7::getSettingState() {
  return &MySettingState;
}

PairingState* DarkNet7::getPairingState() {
  return &MyPairingState;
}

AddressState* DarkNet7::getAddressBookState() {
  return &MyAddressState;
}

Menu3D* DarkNet7::get3DState() {
  return &MyMenu3D;
}

GameOfLife* DarkNet7::getGameOfLifeState() {
  return &MyGameOfLife;
}

CommunicationSettingState* DarkNet7::getCommunicationSettingState() {
  return &MyCommunicationSettings;
}

BadgeInfoState* DarkNet7::getBadgeInfoState() {
  return &MyBadgeInfoState;
}

MCUInfoState* DarkNet7::getMCUInfoState() {
  return &MyMCUInfoState;
}

//Tamagotchi *DarkNet7::getTamagotchiState() {
//	return &MyTamagotchi;
//}

Health* DarkNet7::getHealthState() {
  return &MyHealth;
}

Scan* DarkNet7::getScanState() {
  return &MyScan;
}
