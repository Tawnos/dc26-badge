#pragma once

#include <stdint.h>

namespace cmdc0de {

  enum class RotationType
  {
    PortraitTopLeft = 0,
    LandscapeTopLeft = 1
  };

  static constexpr const uint8_t MyAddressInfoSector = 3; //same sector as settings just first thing
  static constexpr const uint32_t MyAddressInfoOffSet = 0;
  static constexpr const uint8_t SettingSector = 1;
  static constexpr const uint32_t SettingOffset = 0;
  static constexpr const uint8_t StartContactSector = 2;
  static constexpr const uint8_t EndContactSector = 3;

#define START_LANDSCAPE
#ifdef START_LANDSCAPE
  static constexpr const uint32_t DISPLAY_WIDTH = 160;
  static constexpr const uint32_t DISPLAY_HEIGHT = 128;
  static constexpr const uint32_t DISPLAY_OPT_WRITE_ROWS = 128;
  static constexpr const cmdc0de::RotationType START_ROT = cmdc0de::RotationType::LandscapeTopLeft;
#else
  static constexpr const uint32_t DISPLAY_WIDTH = 128;
  static constexpr const uint32_t DISPLAY_HEIGHT = 160;
  static constexpr const uint32_t DISPLAY_OPT_WRITE_ROWS = 128;
  static constexpr const cmdc0de::RotationType START_ROT = cmdc0de::RotationType::PortraitTopLeft;
#endif

  static constexpr const char* sYES = "Yes";
  static constexpr const char* sNO = "No";
  static constexpr const char* NO_DATA_FROM_ESP = "No data returned from ESP, try resetting ESP.";
  static constexpr const char* BLE_CONNECT_FAILED = "BLE Connection failed.";
  static constexpr const char* BLE_DISCONNECTING = "BLE Disconnecting.";
  static constexpr const char* BLE_PAIRING_SUCCESS = "Paired.";
  static constexpr const char* BLE_PAIRING_FAILED = "Failed to Pair.";

  static constexpr const uint16_t BROADCAST_ADDR = 0xFFFF;

  static uint16_t DrawBuffer[DISPLAY_WIDTH * DISPLAY_OPT_WRITE_ROWS]; //120 wide, 10 pixels high, 2 bytes per pixel (uint16_t)
}
