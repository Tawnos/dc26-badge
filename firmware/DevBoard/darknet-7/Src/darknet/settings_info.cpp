#include "settings_info.h"
//
//void SettingsInfo::receiveSignal(MCUToMCU* mcu, const MSGEvent<darknet7::BLEInfectionData>* mevt)
//{
//   uint16_t exposures = mevt->InnerMsg->exposures();
//   uint16_t infections = 0x0;
//   uint32_t unlucky = rand() % 100;
//   infections |= (unlucky < 5) ? (exposures & 0x0004) : 0;
//   infections |= (unlucky < 50) ? (exposures & 0x0020) : 0;
//
//   this->setHealth(infections);
//
//   //Uncomment this to infect other people
//   infections = getHealth();
//   infections &= 0x0004; // Only expose others to measels
//   flatbuffers::FlatBufferBuilder fbb;
//   auto r = darknet7::CreateBLESetInfectionData(fbb, infections);
//   auto e = darknet7::CreateSTMToESPRequest(fbb, 0, darknet7::STMToESPAny_BLESetInfectionData, r.Union());
//   darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, e);
//   mcu->send(fbb);
//
//   return;
//}

bool SettingsInfo::setAgentname(const char name[AGENT_NAME_LENGTH])
{
   strncpy(getSettings()->AgentName, &name[0], AGENT_NAME_LENGTH);
   auto ds = getSettings();
   return writeSettings(*ds);
}

bool SettingsInfo::setHealth(uint16_t v)
{
   auto ds = getSettings();
   if (v == CLEAR_ALL)
   {
      ds->Health = 0;
   }
   else
   {
      ds->Health |= v;
   }
   return writeSettings(*ds);
}

uint16_t SettingsInfo::getHealth()
{
   auto ds = getSettings();
   return (ds->Health & 0xFFFF);
}

bool SettingsInfo::isInfectedWith(uint16_t v)
{
   auto ds = getSettings();
   return ((v & ds->Health) == v);
}

bool SettingsInfo::cure(uint16_t v)
{
   auto ds = getSettings();
   ds->Health = (ds->Health & ~v);
   return writeSettings(*ds);
}

bool SettingsInfo::isNameSet()
{
   return (getSettings()->AgentName[0] != '\0' && getSettings()->AgentName[0] != '_');
}

const char* SettingsInfo::getAgentName()
{
   return getSettings()->AgentName;
}

uint8_t SettingsInfo::getNumContacts()
{
   return getSettings()->NumContacts;
}

SettingsInfo::DataStructure* SettingsInfo::getSettings()
{
   return settings;
}

void SettingsInfo::resetToFactory()
{
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
   //memset(this->StartAddress, 0, SettingsInfo::SIZE);
}

bool SettingsInfo::writeSettings(const DataStructure& ds)
{
   //memset(getSettings(), 0, SettingsInfo::SIZE);
   //
   //auto settingsData = *((uint32_t*)&ds);
   //auto target = (uint32_t*)CurrentAddress;
   //target[0] = SETTING_MARKER;
   //target[1] = settingsData;

   //auto agentStart = &target[2];
   //auto agentName = getSettings()->AgentName;
   //agentStart[0] = *((uint32_t*)&agentName[0]);
   //agentStart[1] = *((uint32_t*)&agentName[4]);
   //agentStart[2] = *((uint32_t*)&agentName[8]);

   if (ds.Magic != SETTING_MARKER)
   {
      return false;
   }
   memmove(settings, &ds, sizeof(DataStructure));
   return true;
}

uint8_t SettingsInfo::setNumContacts(uint8_t num)
{
   if (num > MAX_CONTACTS)
      return MAX_CONTACTS;
   auto ds = getSettings();
   ds->NumContacts = num;
   writeSettings(*ds);
   return num;
}

bool SettingsInfo::setScreenSaverType(uint8_t value)
{
   auto ds = getSettings();
   ds->ScreenSaverType = value & 0xF;
   return writeSettings(*ds);
}

uint8_t SettingsInfo::getScreenSaverType()
{
   auto ds = getSettings();
   return ds->ScreenSaverType;
}

bool SettingsInfo::setScreenSaverTime(uint8_t value)
{
   auto ds = getSettings();
   ds->ScreenSaverTime = value & 0xF;
   return writeSettings(*ds);
}

uint8_t SettingsInfo::getScreenSaverTime()
{
   return getSettings()->ScreenSaverTime;
}

bool SettingsInfo::setSleepTime(uint8_t n)
{
   auto ds = getSettings();
   ds->SleepTimer = n & 0xF;
   return writeSettings(*ds);
}

uint8_t SettingsInfo::getSleepTime()
{
   return getSettings()->SleepTimer;
}