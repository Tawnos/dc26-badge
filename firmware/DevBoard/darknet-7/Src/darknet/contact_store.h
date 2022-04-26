#ifndef KEY_STORE_H
#define KEY_STORE_H

//#include <stm32f4xx_hal.h>
#include "common.h"
#include "contact.h"
#include "my_info.h"
#include "settings_info.h"

class FLASH_LOCKER {
public:
  FLASH_LOCKER() {
#if !defined VIRTUAL_DEVICE
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR);
#endif
  }
  ~FLASH_LOCKER() {
#if !defined VIRTUAL_DEVICE
    HAL_FLASH_Lock();
#endif
  }
};
/////////////////////////////
// Sector 57: erase then rotate though saving SettingInfo
//				  	byte 0: 0xDC
//					byte 1: 0xDC
//					byte 2: 0xDC
//					byte 3: 0xDC
//					byte 4: 0
//					byte 5: <number of contacts>
//					byte 6: bits 0-3: screen save type
//					byte 6: bits 4-7: sleep timer
//					byte 7: bits 0-3: screen saver time
//					byte 7: bits 4-7: ?
//					byte 8-19: Agent Name
// Sector 58:
//				0-87 Contact 0
//				...
//				880-967 Contact 10
//
//	Sector 59:
//				0-87 Contact 11
//				...
//				880-967 Contact 22
//
//	...
//	Sector 63:
//				0-87 Contact 55
//				...
//				880-967 Contact 66
//			980-1020 My Info
//					My Info Address[0-1] = 0xdcdc
//					My Info Address[2-3] = radio unique id
//					My Info Address[4-27] = badge owner private key
//					My Info Address[28-29] = static settings
//[ address book ]
//		Contact
//				[0-3] unique id
//				[4-31] public key (compressed version - 25 (26) bytes)
//				[32-79] Signature (contact signs you're id+public key)
//				[80-91] Agent name
//start address[(every 46 bytes)] = Contact1
/////////////////////////////

class ContactStore {
public:

public:
  ContactStore(
    SettingsInfo settings,
    MyInfo myInfo,
    uint8_t startContactSector = cmdc0de::StartContactSector,
    uint8_t endContactSector = cmdc0de::StartContactSector+1
  ) : Settings(settings),
      StartingContactSector(startContactSector),
      EndContactSector(endContactSector),
      MeInfo(myInfo) {};

  ContactStore(
    uint8_t myAddressInfoSector,
    uint32_t myAddressInfoOffset,
    uint8_t settingSector,
    uint32_t settingOffset,
    uint8_t startContactSector,
    uint8_t endContactSector
  ) : ContactStore(
        SettingsInfo{ SectorToAddress(settingSector) + settingOffset, SectorToAddress(settingSector + 1) },
        MyInfo{SectorToAddress(myAddressInfoSector) + myAddressInfoOffset},
        startContactSector, 
        endContactSector
      ){ }

  MyInfo& getMyInfo() { return MeInfo; }
  SettingsInfo& getSettings() { return Settings; }

  bool init() {
    if (MeInfo.init() && Settings.init()) {
      //version is good, validate keys:
      uint8_t publicKey[PUBLIC_KEY_LENGTH] = { 0 };
      //given our private key, generate our public key and ensure its good
      if (uECC_compute_public_key(MeInfo.getPrivateKey(), &publicKey[0], THE_CURVE)) {
        if (uECC_valid_public_key(&publicKey[0], THE_CURVE) == 1) {
          return true;
        }
      }
    }
    return false;
  }

  bool addContact(uint16_t uid, char agentName[AGENT_NAME_LENGTH], uint8_t key[PUBLIC_KEY_LENGTH], uint8_t sig[SIGNATURE_LENGTH]) {
    uint8_t currentContacts = Settings.getNumContacts();
    uint8_t newNumContacts = Settings.setNumContacts(currentContacts + 1);

    if (newNumContacts == currentContacts) {
      return false;
    }
    Contact c((uint8_t*)0xFFFFFFFF);
    if (getContactAt(currentContacts, c)) {
      c.setUniqueID(uid);
      c.setAgentname(agentName);
      c.setCompressedPublicKey(key);
      c.setPairingSignature(sig);
      return true;
    }
    return false;
  }

  constexpr uint8_t getNumContactsThatCanBeStored() { return MAX_CONTACTS; }

  bool getContactAt(uint16_t numContact, Contact& c) {
    if (numContact < Settings.getNumContacts()) {
      //determine page
      uint16_t sector = StartingContactSector + numContact / CONTACTS_PER_PAGE;
      //valid page is in our range
      if (sector < EndContactSector) {
        uint16_t offSet = numContact % CONTACTS_PER_PAGE;
        uint8_t* sectorAddress = SectorToAddress(sector);
        c.StartAddress = sectorAddress + (offSet * Contact::SIZE);
        return true;
      }
    }
    return false;
  }
  bool findContactByID(uint16_t uid, Contact& c);
  void resetToFactory();

private:
  SettingsInfo Settings{ SectorToAddress(cmdc0de::SettingSector), SectorToAddress(cmdc0de::SettingSector + 1) };
  MyInfo MeInfo;
  uint8_t StartingContactSector;
  uint8_t EndContactSector;
};

#endif
