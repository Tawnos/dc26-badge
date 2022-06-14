#pragma once
#include "contact.h"
#include <stdint.h>
#include <exception>

class MyInfo
{
public:
   //								          0xdcdc		  radio id								         static settings
   static const uint8_t SIZE = sizeof(uint16_t) + sizeof(uint16_t) + PRIVATE_KEY_LENGTH + sizeof(uint16_t);

public:
   MyInfo(uint8_t* startAddress) 
      : startAddress(startAddress)
   {
      // given our private key, generate our public key and ensure its good
      if (!uECC_compute_public_key(getPrivateKey(), publicKey, THE_CURVE)
         || !uECC_valid_public_key(publicKey, THE_CURVE))
      {
         throw std::exception{};
      }
   }

   uint8_t* getCompressedPublicKey()
   {
      uECC_compress(getPublicKey(), &compressedPublicKey[0], THE_CURVE);
      return &compressedPublicKey[0];
   }
   constexpr const uint8_t* getPrivateKey() const { return ((uint8_t*)(startAddress + sizeof(uint16_t) + sizeof(uint16_t))); }
   constexpr const uint8_t* getPublicKey() const { return publicKey; }
   constexpr uint16_t getUniqueID() const { return *((uint16_t*)(startAddress + sizeof(uint16_t))); } 
   constexpr bool isUberBadge() const { return ((getFlags() & 0x1) != 0); }

protected:
   constexpr uint16_t getFlags() const { return *((uint16_t*)(startAddress + sizeof(uint16_t) + sizeof(uint16_t) + PRIVATE_KEY_LENGTH)); } 

private:
   uint8_t* startAddress;
   uint8_t publicKey[PUBLIC_KEY_LENGTH]{ 0 };
   uint8_t compressedPublicKey[PUBLIC_KEY_COMPRESSED_STORAGE_LENGTH]{ 0 };
};
