#pragma once

#include <stdint.h>
#include <crypto/micro-ecc/uECC.h>
#include <cstring>

struct SectorInfo
{
   uint8_t* StartAddress;
   uint32_t Size;
};

#if !defined FLASH_BASE
#define FLASH_BASE 0
#endif

#if !defined VIRTUAL_DEVICE
constexpr uint8_t* SectorToAddress(uint8_t sector)
{
   SectorInfo addresses[] =
   {
         { (uint8_t*)(FLASH_BASE + 0x0000000), 16384 } // (0x4000 16kB) not protected - program
       , { (uint8_t*)(FLASH_BASE + 0x00004000), 16384 } //(0x4000 16kB) not protected - data for settings
       , { (uint8_t*)(FLASH_BASE + 0x00008000), 16384 } //(0x4000 16kB) not protected - data for address
       , { (uint8_t*)(FLASH_BASE + 0x0000c000), 16384 } //(0x4000 16kB) not protected - data
       , { (uint8_t*)(FLASH_BASE + 0x00010000), 65536 } //(0x10000 64kB) not protected - data resources
       , { (uint8_t*)(FLASH_BASE + 0x00020000), 131072 } //(0x20000 128kB) not protected - program
       , { (uint8_t*)(FLASH_BASE + 0x00040000), 131072 } //(0x20000 128kB) not protected - program
       , { (uint8_t*)(FLASH_BASE + 0x00060000), 131072 } //(0x20000 128kB) not protected - program
   };
   return addresses[sector].StartAddress;
}
#endif
#define THE_CURVE uECC_secp192r1()
//should trait this in the future for different curves
static const uint8_t PUBLIC_KEY_LENGTH = 48; //uncompressed size
static const uint8_t PUBLIC_KEY_COMPRESSED_LENGTH = 25; //compressed size
static const uint8_t PUBLIC_KEY_COMPRESSED_STORAGE_LENGTH = 26;
static const uint8_t PRIVATE_KEY_LENGTH = 24;
static const uint8_t DaemonPublic[PUBLIC_KEY_LENGTH] = { 0 };
static const uint8_t SIGNATURE_LENGTH = 48;
static const uint8_t AGENT_NAME_LENGTH = 12;
static const uint8_t CURRENT_VERSION = 0xDC;

// storage sizes, all must be 4 byte aligned
static const uint8_t _SIZE_OF_ID_STORAGE = (4);
static const uint8_t _SIZE_OF_PUBKEY_STORAGE = (28);
static const uint8_t _SIZE_OF_SIG_STORAGE = (48);
static const uint8_t _SIZE_OF_NAME_STORAGE = (12);
static const uint8_t _SIZE_OF_CONTACT = (_SIZE_OF_ID_STORAGE + _SIZE_OF_PUBKEY_STORAGE + _SIZE_OF_SIG_STORAGE + _SIZE_OF_NAME_STORAGE);
static const uint8_t _OFFSET_OF_ID = (0);
static const uint8_t _OFFSET_OF_PUBKEY = (_SIZE_OF_ID_STORAGE);
static const uint8_t _OFFSET_OF_SIG = (_OFFSET_OF_PUBKEY + _SIZE_OF_PUBKEY_STORAGE);
static const uint8_t _OFFSET_OF_AGENT_NAME = (_OFFSET_OF_SIG + _SIZE_OF_SIG_STORAGE);

static const uint8_t MAX_CONTACTS = 186; //16384/88;
static const uint8_t CONTACTS_PER_PAGE = MAX_CONTACTS; //for STM32F411 1 sector is being used of 16K so numbers are the same

struct alignas(32) StoredContact
{
   const uint32_t Id;
   const char PublicKey[PUBLIC_KEY_COMPRESSED_LENGTH];
   const char Signature[SIGNATURE_LENGTH];
   const char AgentName[AGENT_NAME_LENGTH];
};

//		Contact
//				[0-3] unique id
//				[4-31] public key (compressed version - 25 (26) bytes)
//				[32-79] Signature (contact signs your id+public key)
//				[80-91] Agent name
class Contact
{
public:
   friend class ContactStore;

   void setUniqueID(uint16_t id)
   {
#if !defined VIRTUAL_DEVICE
      FLASH_LOCKER f;
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, StartAddress, id);
#else
      ((uint16_t*)StartAddress)[0] = id;
#endif
   }

   void setAgentname(const char name[AGENT_NAME_LENGTH])
   {
      //int len = strlen(name);
      auto s = (uint32_t*)StartAddress + _OFFSET_OF_AGENT_NAME;
#if !defined VIRTUAL_DEVICE

      FLASH_LOCKER f;
      uint8_t* s = StartAddress + _OFFSET_OF_AGENT_NAME;
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s, (*((uint32_t*)&name[0])));
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 4, (*((uint32_t*)&name[4])));
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, s + 8, (*((uint32_t*)&name[8])));
#else
      memcpy(s, name, AGENT_NAME_LENGTH);
#endif
   }

   void setCompressedPublicKey(const uint8_t key[PUBLIC_KEY_COMPRESSED_LENGTH]);
   void setPairingSignature(const uint8_t sig[SIGNATURE_LENGTH]);

   Contact(uint8_t* startAddr = 0) :
      StartAddress(startAddr)
   {}

   uint16_t getUniqueID() { return *((uint16_t*)StartAddress); }
   const char* getAgentName() { return ((char*)(StartAddress + _OFFSET_OF_AGENT_NAME)); }
   uint8_t* getCompressedPublicKey() { return ((uint8_t*)(StartAddress + _OFFSET_OF_PUBKEY)); }
   void getUnCompressedPublicKey(uint8_t key[PUBLIC_KEY_LENGTH]) { uECC_decompress(getCompressedPublicKey(), &key[0], THE_CURVE); }
   uint8_t* getPairingSignature() { return ((uint8_t*)(StartAddress + _OFFSET_OF_SIG)); }

protected:
   uint8_t* StartAddress;
};