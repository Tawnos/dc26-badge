#include "mcu_to_mcu.h"
#include "stm_to_esp_generated.h"
#include "esp_to_stm_generated.h"
#include "command_handler.h"
extern "C" {
#include "checksum.h"
}

#define LOGTAG "MCUToMCUTask"

#if !defined VIRTUAL_DEVICE
#include "dc26.h"
#include "dc26_ble/ble.h"
const char* MCUToMCUTask::LOGTAG = "MCUToMCUTask";
static uint8_t ESPToSTMBuffer[MCUToMCUTask::ESP_TO_STM_MSG_QUEUE_SIZE * MCUToMCUTask::ESP_TO_STM_MSG_ITEM_SIZE];
static StaticQueue_t OutgoingQueue;
static QueueHandle_t OutgoingQueueHandle = nullptr;
static xTaskHandle UARTSendTaskhandle = 0;
#endif


const darknet7::ESPToSTM* MCUToMCUTask::Message::asESPToSTM()
{
   return darknet7::GetSizePrefixedESPToSTM(&MessageData[ENVELOP_HEADER]);
}

const darknet7::STMToESPRequest* MCUToMCUTask::Message::asSTMToESP()
{
   return darknet7::GetSizePrefixedSTMToESPRequest(
      &MessageData[ENVELOP_HEADER]);
}

const darknet7::STMToESPRequest* MCUToMCUTask::Message::asSTMToESPVerify()
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


bool MCUToMCUTask::Message::read(const uint8_t* data, uint32_t dataSize)
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

void MCUToMCUTask::Message::set(uint16_t sf, uint16_t crc, uint8_t* data)
{
   SizeAndFlags = sf;
   Crc16 = crc;
   MessageData[0] = sf & 0xFF;
   MessageData[1] = (sf & 0xFF00) >> 8;
   MessageData[2] = Crc16 & 0xFF;
   MessageData[3] = (Crc16 & 0xFF00) >> 8;
   memcpy(&MessageData[ENVELOP_HEADER], data, getDataSize());
}

void MCUToMCUTask::Message::setFlag(uint16_t flags)
{
   SizeAndFlags |= flags;
}

bool MCUToMCUTask::Message::checkFlags(uint16_t flags)
{
   return (SizeAndFlags & flags) == flags;
}

/////////////////////////


//
//void MCUToMCUTask::onStart()
//{
//   xTaskCreate(uart_send, "uart_send", 6048, NULL, 5, &UARTSendTaskhandle);
//}
//
//void MCUToMCUTask::onStop()
//{
//   xTaskHandle temp = UARTSendTaskhandle;
//   UARTSendTaskhandle = nullptr;
//   vTaskDelete(temp);
//}

//
//MCUToMCUTask::~MCUToMCUTask()
//{
//   uart_driver_delete(UART_NUM_1);
//}

void MCUToMCUTask::send(const flatbuffers::FlatBufferBuilder& fbb)
{
   uint8_t* msg = fbb.GetBufferPointer();
   uint32_t size = fbb.GetSize();
   uint16_t crc = crc_16(msg, size);
   ESP_LOGI(LOGTAG, "Queuing size %d, crc %d\n", size, crc);
   assert(size < MAX_MESSAGE_SIZE);
   Message* m = new Message();
   m->set(size, crc, msg);
   uartQueue.push(m);
   delete m;
   //xQueueSend(OutgoingQueueHandle, (void*)&m, (TickType_t)100);
}


int32_t MCUToMCUTask::processMessage(const uint8_t* data, uint32_t size)
{
   ESP_LOGI(LOGTAG, "Process Message");
   Message* m = new Message();
   int32_t retVal = 0;
   if (m->read(data, size))
   {
      auto msg = m->asSTMToESPVerify();
      if (msg)
      {
         retVal = m->getMessageSize();
         ESP_LOGI(LOGTAG, "MsgType %d", msg->Msg_type());
         //ESP_LOGI(LOGTAG, darknet7::EnumNamesSTMToESPAny(msg->Msg_type()));
         switch (msg->Msg_type())
         {
         case darknet7::STMToESPAny_StopAP:
         case darknet7::STMToESPAny_SetupAP:
         case darknet7::STMToESPAny_CommunicationStatusRequest:
         case darknet7::STMToESPAny_ESPRequest:
         case darknet7::STMToESPAny_WiFiScan:
         case darknet7::STMToESPAny_WiFiNPCInteract:
            ESP_LOGI(LOGTAG, "sending to cmd handler");
            CmdHandler->getMessageQueue().push(m);
            //xQueueSend(CmdHandler->getQueueHandle(), (void*)&m, (TickType_t)0);
            ESP_LOGI(LOGTAG, "after send to cmd handler");
            break;
#if !defined VIRTUAL_DEVICE
         case darknet7::STMToESPAny_BLEAdvertise:
         case darknet7::STMToESPAny_BLESetDeviceName:
         case darknet7::STMToESPAny_BLEGetInfectionData:
         case darknet7::STMToESPAny_BLESetExposureData:
         case darknet7::STMToESPAny_BLESetInfectionData:
         case darknet7::STMToESPAny_BLESetCureData:
         case darknet7::STMToESPAny_BLEScanForDevices:
         case darknet7::STMToESPAny_BLEPairWithDevice:
         case darknet7::STMToESPAny_BLESendPINConfirmation:
         case darknet7::STMToESPAny_BLESendDataToDevice:
         case darknet7::STMToESPAny_BLESendDNPairComplete:
         case darknet7::STMToESPAny_BLEDisconnect:
            ESP_LOGI(LOGTAG, "sending to bluetooth task");
            xQueueSend(getBLETask().getQueueHandle(), (void*)&m, (TickType_t)0);
            ESP_LOGI(LOGTAG, "after send to bluetooth task");
            break;
#endif
         default:
            break;
         }
      }
      else
      {
         retVal = -1;
      }
   }
   else
   {
      delete m;
   }
   return retVal;
}

void MCUToMCUTask::run(std::stop_token stoken)
{
   //esp_log_level_set(LOGTAG, ESP_LOG_INFO);
   uint8_t dataBuf[MAX_MESSAGE_SIZE * 2] = { 0 };
   size_t receivePtr = 0;
   while (!stoken.stop_requested())
   {
#if !defined VIRTUAL_DEVICE
      receivePtr += uart_read_bytes(UART_NUM_1, &dataBuf[0], sizeof(dataBuf), 100);
#endif
      if (receivePtr > ENVELOP_HEADER)
      {
         //ESP_LOG_BUFFER_HEXDUMP(LOGTAG, &dataBuf[0], receivePtr, ESP_LOG_INFO);
         int32_t consumed = processMessage(&dataBuf[0], receivePtr);
         if (consumed > 0)
         {
            ESP_LOGI(LOGTAG, "process Msg: consumed=%d total=%d", consumed, receivePtr);
            receivePtr -= consumed;
            assert(receivePtr < sizeof(dataBuf));
            memmove(&dataBuf[0], &dataBuf[consumed], consumed);
         }
         else if (consumed < 0)
         {
            ESP_LOGI(LOGTAG, "error process message resetting queue");
            memset(&dataBuf[0], 0, sizeof(dataBuf));
         }
         else /*0*/
         {
         }
      }
      std::this_thread::sleep_for(50ms);
   }
}

