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

#include "menus/menu_state.h"
#include "menus/setting_state.h"
#include "menus/pairing_state.h"
#include "menus/AddressState.h"
#include "menus/badge_info_state.h"
#include "menus/SendMsgState.h"
#include "menus/communications_settings.h"
#include "menus/health.h"
#include "art/images.h"

#if !defined VIRTUAL_DEVICE
#include "messaging/stm_to_esp_generated.h"
#include "mcu_to_mcu.h"
#include "menus/mcu_info.h"
#include "menus/menu3d.h"
#include "menus/GameOfLife.h"
#include "menus/sao_menu.h"
#include "menus/scan.h"
#include "menus/tamagotchi.h"
#include "menus/test_state.h"
#endif

using cmdc0de::ErrorType;
using cmdc0de::DrawBufferNoBuffer;
using cmdc0de::GUIListData;
using cmdc0de::GUIListItemData;
using cmdc0de::GUI;
using cmdc0de::RGBColor;

static MenuState MyMenu;
static SendMsgState MySendMsgState;

static SettingState MySettingState;
static AddressState MyAddressState;
static CommunicationSettingState MyCommunicationSettings;
static BadgeInfoState MyBadgeInfoState;
static Health MyHealth;

#if !defined VIRTUAL_DEVICE
static MCUInfoState MyMCUInfoState;
static TestState MyTestState;
//static Tamagotchi MyTamagotchi;
static Menu3D MyMenu3D;
static GameOfLife MyGameOfLife;
static Scan MyScan;
static PairingState MyPairingState;
static SAO MySAO;
#endif

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

cmdc0de::DisplayMessageState* DarkNet7::getDisplayMessageState(cmdc0de::StateBase* bm, const char* message, uint16_t timeToDisplay) {
  DMS.setMessage(message);
  DMS.setNextState(bm);
  DMS.setTimeInState(timeToDisplay);
  DMS.setDisplay(Display);
  return &DMS;
}

uint32_t DarkNet7::nextSeq() {
  return ++SequenceNum;
}

MenuState* DarkNet7::getDisplayMenuState() {
  return &MyMenu;
}

SendMsgState* DarkNet7::getSendMsgState() {
  return &MySendMsgState;
}

SettingState* DarkNet7::getSettingState() {
  return &MySettingState;
}

AddressState* DarkNet7::getAddressBookState() {
  return &MyAddressState;
}
CommunicationSettingState* DarkNet7::getCommunicationSettingState() {
  return &MyCommunicationSettings;
}

BadgeInfoState* DarkNet7::getBadgeInfoState() {
  return &MyBadgeInfoState;
}

Health* DarkNet7::getHealthState() {
  return &MyHealth;
}

#if !defined VIRTUAL_DEVICE

MCUInfoState* DarkNet7::getMCUInfoState() {
  return &MyMCUInfoState;
}

//Tamagotchi *DarkNet7::getTamagotchiState() {
//	return &MyTamagotchi;
//}

SAO* DarkNet7::getSAOMenuState() {
  return &MySAO;
}

TestState* DarkNet7::getTestState() {
  return &MyTestState;
}

PairingState* DarkNet7::getPairingState() {
  return &MyPairingState;
}


Menu3D* DarkNet7::get3DState() {
  return &MyMenu3D;
}

GameOfLife* DarkNet7::getGameOfLifeState() {
  return &MyGameOfLife;
}

Scan* DarkNet7::getScanState() {
  return &MyScan;
}
#endif