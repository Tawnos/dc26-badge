#pragma once
#include <stdint.h>
#include <messaging/mcu_message.h>
#include <messaging/stm_to_esp_generated.h>
#include <messaging/esp_to_stm_generated.h>

//We listen for the for our envelop portion of our message which is: 4 bytes:
// bits 0-10 is the size of the message coming max message size = 1024
// bit 11: reserved
// bit 12: reserved
// bit 13: reserved
// bit 14: reserved
// bit 15: reserved
// bit 16-31: CRC 16 of entire message
//set up for our 4 byte envelop header
class MCUMessage
{
   inline static const char* LOGTAG = "MCUMessage";

public:
   constexpr static const uint16_t MESSAGE_FLAG_TRANSMITTED = 0x8000; //bit 15
   constexpr static const int ENVELOP_HEADER = 4;
   constexpr static const int ENVELOP_HEADER_SIZE_MASK = 0x7FF;
   constexpr static const int MAX_MESSAGE_SIZE = 300;
   constexpr static const int TOTAL_MESSAGE_SIZE = MAX_MESSAGE_SIZE + ENVELOP_HEADER;
public:
   MCUMessage() = default;
   MCUMessage(uint16_t sf, uint16_t crc, uint8_t* data)
      : SizeAndFlags(sf), Crc16(crc)
   {
      MessageData[0] = sf & 0xFF;
      MessageData[1] = (sf & 0xFF00) >> 8;
      MessageData[2] = Crc16 & 0xFF;
      MessageData[3] = (Crc16 & 0xFF00) >> 8;
      memcpy(&MessageData[ENVELOP_HEADER], data, getDataSize());
   }

   bool read(const uint8_t* data, uint32_t dataSize);

   const darknet7::ESPToSTM* asESPToSTM();
   bool verifyESPToSTM();

   const darknet7::STMToESPRequest* asSTMToESP();
   const darknet7::STMToESPRequest* asSTMToESPVerify(); 

   void setFlag(uint16_t flags) { SizeAndFlags |= flags; }
   bool checkFlags(uint16_t flags) const { return (SizeAndFlags & flags) == flags; }
   uint16_t getMessageSize() const { return getDataSize() + ENVELOP_HEADER; }
   uint16_t getSizeAndFlags() const { return SizeAndFlags; }
protected:
   //HAL_StatusTypeDef transmit(UART_HandleTypeDef* huart);

   uint16_t getDataSize() const { return SizeAndFlags & ENVELOP_HEADER_SIZE_MASK; }
   static uint16_t getDataSize(uint16_t s) { return s & ENVELOP_HEADER_SIZE_MASK; }
private:
   uint16_t SizeAndFlags = 0;
   uint16_t Crc16 = 0;
   uint8_t MessageData[MAX_MESSAGE_SIZE] = { 0 };
};
