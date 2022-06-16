#include "contact_store.h"
#include <string.h>
#include <crypto/micro-ecc/uECC.h>
#include <messaging/stm_to_esp_generated.h>
#include <messaging/esp_to_stm_generated.h>


//#include "../darknet7.h"


// MyInfo
//===========================================================


/////////////////////////////////////////////////////////////////////////////////

void Contact::setCompressedPublicKey(const uint8_t key[PUBLIC_KEY_COMPRESSED_LENGTH])
{
   uint8_t* s = StartAddress + _OFFSET_OF_PUBKEY;
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
#else
   memcpy(s, key, PUBLIC_KEY_COMPRESSED_LENGTH);
#endif
}

void Contact::setPairingSignature(const uint8_t sig[SIGNATURE_LENGTH])
{
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
#else
   memcpy(s, sig, SIGNATURE_LENGTH);
#endif
}

//====================================================

void ContactStore::resetToFactory()
{
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

      for (int i = StartingContactSector; i < EndContactSector; ++i)
      {
         HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
      }
   }
#endif
}

bool ContactStore::findContactByID(uint16_t uid, Contact& c)
{
   if (uid == 0)
      return false;
   uint8_t currentContacts = Settings.getNumContacts();
   for (int i = 0; i < currentContacts; i++)
   {
      if (getContactAt(i, c))
      {
         if (uid == c.getUniqueID())
         {
            return true;
         }
      }
   }
   c.StartAddress = 0;
   return false;
}

