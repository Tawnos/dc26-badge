#pragma once
#include "contact.h"
class MyInfo
{
public:
    //								          0xdcdc			        radio id								               static settings
    static const uint8_t SIZE = sizeof(uint16_t) + sizeof(uint16_t) + PRIVATE_KEY_LENGTH + sizeof(uint16_t);

public:
    MyInfo(uint8_t* startAddress)
        : StartAddress(startAddress) {}

    bool init() { return (*(uint16_t *)StartAddress) == 0xdcdc; }
    uint8_t *getCompressedPublicKey()
    {
        uECC_compress(getPublicKey(), &compressedPublicKey[0], THE_CURVE);
        return &compressedPublicKey[0];
    }
    uint8_t *getPrivateKey() { return ((uint8_t *)(StartAddress + sizeof(uint16_t) + sizeof(uint16_t))); }
    uint8_t *getPublicKey()
    {
        // given our private key, generate our public key and ensure its good
        if (uECC_compute_public_key(getPrivateKey(), &publicKey[0], THE_CURVE))
        {
            if (uECC_valid_public_key(&publicKey[0], THE_CURVE) == 1)
            {
                return &publicKey[0];
            }
        }
        return 0;
    }
    uint16_t getUniqueID() { return *((uint16_t *)(StartAddress + sizeof(uint16_t))); }
    bool isUberBadge() { return ((getFlags() & 0x1) != 0); }

protected:
    uint16_t getFlags() { return *((uint16_t *)(StartAddress + sizeof(uint16_t) + sizeof(uint16_t) + PRIVATE_KEY_LENGTH)); }

private:
    uint8_t* StartAddress{nullptr};
    uint8_t publicKey[PUBLIC_KEY_LENGTH]{0};
    uint8_t compressedPublicKey[PUBLIC_KEY_COMPRESSED_STORAGE_LENGTH]{0};
};
