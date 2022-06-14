#pragma once
//#include "darknet7.h"
#include "contact.h"
#include "mcu_to_mcu.h"
#include <messaging/stm_to_esp_generated.h>
#include <messaging/esp_to_stm_generated.h>
/*
 *
  InfectionID				vector		remedy			permanence		contraction%
  Clear Bit (Bit:0)  		Wireless  	Operative Only						100%
  Avian Flu (Bit: 1)		Wireless	AGTAGAAACAAGG		Recurring		12%
  Measles (Bit: 2)		Wireless	GTCAGTTCCACAT		Curable			90%
  Tetanus (Bit: 3)		BLE GATT	GAGGTGCAGCTGG		Recurring		50%
  Polio (Bit: 4)			BLE GATT	ATTCTAACCATGG		Curable			13%
  Plague (Bit: 5)			BLE GATT	AAGAGTATAATCG / DN Table Curable	50%
  Toxoplasmosis (Bit: 6)  BLE GATT	CCTAAACCCTGAA		Recurring		20%
  Chlamydia (Bit: 7) 		Add-On		GTATTAGTATTTG		Curable			50%
  Herpes (Bit: 8)			Add-On		GATCGTTATTCCC		Recurring		30%
 *
 *
 */
class SettingsInfo
{
public:
   enum
   {
      CLEAR_ALL = 0x1,
      AVIAN_FLU = 0x2,
      MEASLES = 0x4,
      TETANUS = 0x8,
      POLIO = 0x10,
      PLAGUE = 0x20,
      TOXOPLASMOSIS = 0x40,
      CHLAMYDIA = 0x80,
      HERPES = 0x100
   };
   static const uint32_t SETTING_MARKER = 0xDCDCDCDC;
   static const uint32_t SETTING_MARKER_LENGTH = 4;
   static const uint8_t SIZE = 8 + AGENT_NAME_LENGTH;
   struct DataStructure
   {
      uint32_t Health : 12;
      uint32_t NumContacts : 8;
      uint32_t ScreenSaverType : 4;
      uint32_t SleepTimer : 4;
      uint32_t ScreenSaverTime : 4;

      char AgentName[AGENT_NAME_LENGTH];
   };

   SettingsInfo(MCUToMCU* mcu, uint8_t* startAddress, uint8_t* endAddress)
      : StartAddress(startAddress),
      CurrentAddress(startAddress),
      EndAddress(endAddress)
   {

      const MSGEvent<darknet7::BLEInfectionData>* removebob = 0;
      // mcu->getBus().addListener(this, removebob, mcu);

      auto foundSettingsMarker = false;
      for (uint8_t* addr = StartAddress; addr < EndAddress; addr += SettingsInfo::SIZE)
      {
         uint32_t value = *((uint32_t*)addr);
         if (value == SETTING_MARKER)
         {
            CurrentAddress = addr;
            //const char* AgentNameAddr = ((const char*)(CurrentAddress + sizeof(uint32_t) + sizeof(uint32_t)));
            //strncpy(&AgentName[0], AgentNameAddr, sizeof(AgentName));
            foundSettingsMarker = true;
         }
      }

      if (!foundSettingsMarker)
      {
         //couldn't find DS
         CurrentAddress = EndAddress;

         //0x1FE; //all the virus
         DataStructure ds{
            .Health = 0,
            .NumContacts = 0,
            .ScreenSaverType = 0,
            .SleepTimer = 3,
            .ScreenSaverTime = 1
         };
         writeSettings(ds);
      }
   }

   void receiveSignal(MCUToMCU*, const MSGEvent<darknet7::BLEInfectionData>* mevt);
   uint32_t getVersion();
   uint8_t setNumContacts(uint8_t n);
   uint8_t getNumContacts();
   bool setScreenSaverType(uint8_t value);
   uint8_t getScreenSaverType();
   bool setScreenSaverTime(uint8_t value);
   uint8_t getScreenSaverTime();
   bool setSleepTime(uint8_t n);
   uint8_t getSleepTime();
   const char* getAgentName();
   bool isNameSet();
   bool setAgentname(const char name[AGENT_NAME_LENGTH]);
   void resetToFactory();
   bool setHealth(uint16_t v);
   uint16_t getHealth();
   bool isInfectedWith(uint16_t v);
   bool cure(uint16_t v);

protected:
   bool writeSettings(const DataStructure& ds);
   DataStructure* getSettings();

private:
   uint8_t* StartAddress;
   uint8_t* EndAddress;
   uint8_t* CurrentAddress;
};
