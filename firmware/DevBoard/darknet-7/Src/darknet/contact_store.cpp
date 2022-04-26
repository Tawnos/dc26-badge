#include "contact_store.h"
#include <string.h>
#include <crypto/micro-ecc/uECC.h>
#if !defined VIRTUAL_DEVICE
#include "mcu_to_mcu.h"
#include "messaging/stm_to_esp_generated.h"
#include "messaging/esp_to_stm_generated.h"
#endif
#include "../darknet7.h"

void SettingsInfo::receiveSignal(MCUToMCU*, const MSGEvent<darknet7::BLEInfectionData>* mevt) {
  uint16_t exposures = mevt->InnerMsg->exposures();
  uint16_t infections = 0x0;
  uint32_t unlucky = rand() % 100;
  infections |= (unlucky < 5) ? (exposures & 0x0004) : 0;
  infections |= (unlucky < 50) ? (exposures & 0x0020) : 0;

  this->setHealth(infections);

  //Uncomment this to infect other people
  infections = DarkNet7::instance->getContacts().getSettings().getHealth();
  infections &= 0x0004; // Only expose others to measels
  flatbuffers::FlatBufferBuilder fbb;
  auto r = darknet7::CreateBLESetInfectionData(fbb, infections);
  auto e = darknet7::CreateSTMToESPRequest(fbb, 0, darknet7::STMToESPAny_BLESetInfectionData, r.Union());
  darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, e);
  DarkNet7::instance->getMcuToMcu().send(fbb);

  return;
}

bool SettingsInfo::init() {
  const MSGEvent<darknet7::BLEInfectionData>* removebob = 0;
#if !defined VIRTUAL_DEVICE
  DarkNet7::instance->getMcuToMcu().getBus().addListener(this, removebob, &DarkNet7::instance->getMcuToMcu());
#endif

  for (uint8_t* addr = StartAddress; addr < EndAddress; addr += SettingsInfo::SIZE) {
    uint32_t value = *((uint32_t*)addr);
    if (value == SETTING_MARKER) {
      CurrentAddress = addr;
      const char* AgentNameAddr = ((const char*)(CurrentAddress + sizeof(uint32_t) + sizeof(uint32_t)));
      strncpy(&AgentName[0], AgentNameAddr, sizeof(AgentName));
      return true;
    }
  }
  //couldn't find DS
  CurrentAddress = EndAddress;
  DataStructure ds;
  ds.Health = 0; //0x1FE; //all the virus
  ds.ScreenSaverTime = 1;
  ds.ScreenSaverType = 0;
  ds.SleepTimer = 3;
  ds.NumContacts = 0;
  return writeSettings(ds);
}

bool SettingsInfo::setAgentname(const char name[AGENT_NAME_LENGTH]) {
  strncpy(&AgentName[0], &name[0], sizeof(AgentName));
  DataStructure ds = getSettings();
  return writeSettings(ds);
}

bool SettingsInfo::setHealth(uint16_t v) {
  DataStructure ds = getSettings();
  if (v == CLEAR_ALL) {
    ds.Health = 0;
  }
  else {
    ds.Health |= v;
  }
  return writeSettings(ds);
}

uint16_t SettingsInfo::getHealth() {
  DataStructure ds = getSettings();
  return (ds.Health & 0xFFFF);
}

bool SettingsInfo::isInfectedWith(uint16_t v) {
  DataStructure ds = getSettings();
  return ((v & ds.Health) == v);
}

bool SettingsInfo::cure(uint16_t v) {
  DataStructure ds = getSettings();
  ds.Health = (ds.Health & ~v);
  return writeSettings(ds);
}

bool SettingsInfo::isNameSet() {
  return (AgentName[0] != '\0' && AgentName[0] != '_');
}

const char* SettingsInfo::getAgentName() {
  return &AgentName[0];
}

uint32_t SettingsInfo::getVersion() {
  return *((uint32_t*)CurrentAddress);
}

uint8_t SettingsInfo::getNumContacts() {
  DataStructure ds = getSettings();
  return ds.NumContacts;
}

SettingsInfo::DataStructure SettingsInfo::getSettings() {
  return *((SettingsInfo::DataStructure*)(CurrentAddress + sizeof(uint32_t)));
}

void SettingsInfo::resetToFactory() {
#if !defined VIRTUAL_DEVICE
  {
    FLASH_LOCKER f;
    uint32_t sectorError = 0;
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Sector = this->SettingSector;
    EraseInitStruct.Banks = 0;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.NbSectors = 1;
    HAL_FLASHEx_Erase(&EraseInitStruct, &sectorError);
  }
#endif
  init();
}

bool SettingsInfo::writeSettings(const DataStructure& ds) {
#if !defined VIRTUAL_DEVICE

  FLASH_LOCKER f;
  uint32_t startNewAddress = CurrentAddress + SettingsInfo::SIZE;
  uint32_t endNewAddress = startNewAddress + SettingsInfo::SIZE;
  if (endNewAddress >= getEndAddress()) {
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Sector = this->SettingSector;
    EraseInitStruct.Banks = 0;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.NbSectors = 1;
    uint32_t SectorError = 0;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
      return false;
    }
    CurrentAddress = StartAddress;
    startNewAddress = CurrentAddress;
    endNewAddress = CurrentAddress + SettingsInfo::SIZE;
  }
  else {
    //zero out the one we were on
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, CurrentAddress, 0); //2
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, CurrentAddress + 4, 0); //4
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, CurrentAddress + 8, 0);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, CurrentAddress + 12, 0);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, CurrentAddress + 16, 0);
    CurrentAddress = startNewAddress;
  }
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, CurrentAddress, SETTING_MARKER);
  uint32_t data = *((uint32_t*)&ds);
  if (HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (CurrentAddress + sizeof(uint32_t)), data)) {
    uint32_t agentStart = CurrentAddress + sizeof(uint32_t) + sizeof(uint32_t);
    if (HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, agentStart, (*((uint32_t*)&AgentName[0])))) {
      if (HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, agentStart + 4, (*((uint32_t*)&AgentName[4])))) {
        if (HAL_OK
          == HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, agentStart + 8, (*((uint32_t*)&AgentName[8])))) {
          return true;
        }
      }
    }
  }
  return false;
#endif
  return false;
}

uint8_t SettingsInfo::setNumContacts(uint8_t num) {
  if (num > MAX_CONTACTS)
    return MAX_CONTACTS;
  DataStructure ds = getSettings();
  ds.NumContacts = num;
  writeSettings(ds);
  return num;
}

bool SettingsInfo::setScreenSaverType(uint8_t value) {
  DataStructure ds = getSettings();
  ds.ScreenSaverType = value & 0xF;
  return writeSettings(ds);
}

uint8_t SettingsInfo::getScreenSaverType() {
  DataStructure ds = getSettings();
  return ds.ScreenSaverType;
}

bool SettingsInfo::setScreenSaverTime(uint8_t value) {
  DataStructure ds = getSettings();
  ds.ScreenSaverTime = value & 0xF;
  return writeSettings(ds);
}

uint8_t SettingsInfo::getScreenSaverTime() {
  return getSettings().ScreenSaverTime;
}

bool SettingsInfo::setSleepTime(uint8_t n) {
  DataStructure ds = getSettings();
  ds.SleepTimer = n & 0xF;
  return writeSettings(ds);
}

uint8_t SettingsInfo::getSleepTime() {
  return getSettings().SleepTimer;
}

// MyInfo
//===========================================================


/////////////////////////////////////////////////////////////////////////////////

void Contact::setCompressedPublicKey(const uint8_t key1[PUBLIC_KEY_COMPRESSED_LENGTH]) {
  uint8_t* s = StartAddress + _OFFSET_OF_PUBKEY;
  uint8_t key[PUBLIC_KEY_COMPRESSED_STORAGE_LENGTH];
  memset(&key[0], 0, sizeof(key)); //set array to 0
  memcpy(&key[0], &key1[0], PUBLIC_KEY_COMPRESSED_LENGTH); //copy over just the 25 bytes of the compressed public key
#if !defined VIRTUAL_DEVICE
  FLASH_LOCKER f;
  //store all bits
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s, (*((uint32_t*)&key[0])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 4, (*((uint32_t*)&key[4])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 8, (*((uint32_t*)&key[8])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 12, (*((uint32_t*)&key[12])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 16, (*((uint32_t*)&key[16])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 20, (*((uint32_t*)&key[20])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, s + 24, (*((uint32_t*)&key[24])));
#endif
}

void Contact::setPairingSignature(const uint8_t sig[SIGNATURE_LENGTH]) {
  uint8_t* s = StartAddress + _OFFSET_OF_SIG;
#if !defined VIRTUAL_DEVICE
  FLASH_LOCKER f;
  //for(uint32_t i=0;i<sizeof(sig);i+=4) {
  //	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s+i, (*((uint32_t *) &sig[i])));
  //}
  //if you look at the assembler being generated the loop will only run twice so we'll run roll it due to time
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 0, (*((uint32_t*)&sig[0])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 4, (*((uint32_t*)&sig[4])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 8, (*((uint32_t*)&sig[8])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 12, (*((uint32_t*)&sig[12])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 16, (*((uint32_t*)&sig[16])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 20, (*((uint32_t*)&sig[20])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 24, (*((uint32_t*)&sig[24])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 28, (*((uint32_t*)&sig[28])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 32, (*((uint32_t*)&sig[32])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 36, (*((uint32_t*)&sig[36])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 40, (*((uint32_t*)&sig[40])));
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 44, (*((uint32_t*)&sig[44])));
#endif
}

//====================================================

void ContactStore::resetToFactory() {
  getSettings().resetToFactory();
#if !defined VIRTUAL_DEVICE
  {
    FLASH_LOCKER f;
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Sector = StartingContactSector;
    EraseInitStruct.Banks = 0;
    EraseInitStruct.NbSectors = 1;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    uint32_t SectorError = 0;

    for (int i = StartingContactSector; i < EndContactSector; ++i) {
      HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
    }
  }
#endif
}

bool ContactStore::findContactByID(uint16_t uid, Contact& c) {
  if (uid == 0)
    return false;
  uint8_t currentContacts = Settings.getNumContacts();
  for (int i = 0; i < currentContacts; i++) {
    if (getContactAt(i, c)) {
      if (uid == c.getUniqueID()) {
        return true;
      }
    }
  }
  c.StartAddress = 0;
  return false;
}

