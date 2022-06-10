#include "mcu_message.h"
#include "../../esp-wroom-32/dc26-esp/components/libcrc/include/checksum.h"

#define ESP_LOGI(tag, string) //printf("%s: %s", tag, string);

bool MCUMessage::read(const uint8_t* data, uint32_t dataSize)
{
   SizeAndFlags = data[0];
   SizeAndFlags |= ((uint16_t)data[1]) << 8;
   Crc16 = data[2];
   Crc16 |= ((uint16_t)data[3]) << 8;
   ESP_LOGI(LOGTAG, "buffer size needs to be: sizeFlags (%u) size (%u) crc(%u)",
      (uint32_t)SizeAndFlags, (uint32_t)getDataSize(), (uint32_t)Crc16);
   assert(getDataSize() < MAX_MESSAGE_SIZE);
   //calc crc
   //uint16_t crc = crc16_le(0, &data[ENVELOP_HEADER], getDataSize());
   uint16_t crc = crc_16(&data[ENVELOP_HEADER], getDataSize());
   if (crc != Crc16)
   {
      ESP_LOGI(LOGTAG, "CRC's don't match calced: %d, STM %d", crc, Crc16);
   }
   else
   {
      if (getMessageSize() <= dataSize)
      {
         ESP_LOGI(LOGTAG, "CRC's match calced: %d, STM %d", crc, Crc16);
         ESP_LOGI(LOGTAG, "coping data to message");
         memcpy(&MessageData[0], &data[0], getMessageSize());
         return true;
      }
   }
   return false;
}


const darknet7::ESPToSTM* MCUMessage::asESPToSTM()
{
   return darknet7::GetSizePrefixedESPToSTM(&MessageData[ENVELOP_HEADER]);
}

const darknet7::STMToESPRequest* MCUMessage::asSTMToESP()
{
   return darknet7::GetSizePrefixedSTMToESPRequest(&MessageData[ENVELOP_HEADER]);
}

const darknet7::STMToESPRequest* MCUMessage::asSTMToESPVerify()
{
   flatbuffers::Verifier v(&MessageData[ENVELOP_HEADER], getDataSize());
   if (darknet7::VerifySizePrefixedSTMToESPRequestBuffer(v))
   {
      return asSTMToESP();
   }
   else
   {
      ESP_LOGI(LOGTAG, "Failed to verify message");
      return nullptr;
   }
}

