#include "mcu_to_mcu.h"
#include "command_handler.h"
extern "C" {
#include "checksum.h"
}

#if !defined VIRTUAL_DEVICE
#include "dc26.h"
#include "dc26_ble/ble.h"
static uint8_t ESPToSTMBuffer[MCUToMCUTask::ESP_TO_STM_MSG_QUEUE_SIZE * MCUToMCUTask::ESP_TO_STM_MSG_ITEM_SIZE];
static StaticQueue_t OutgoingQueue;
static QueueHandle_t OutgoingQueueHandle = nullptr;
static xTaskHandle UARTSendTaskhandle = 0;
#endif


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
   assert(size < MCUMessage::MAX_MESSAGE_SIZE);

   auto m = new MCUMessage{ (uint16_t)size, crc, msg };
   //xQueueSend(OutgoingQueueHandle, (void*)&m, (TickType_t)100);
   auto mPlaced = getQueue().push(m);
   assert(m != mPlaced);
   delete m;
}


int32_t MCUToMCUTask::processMessage(const uint8_t* data, uint32_t size, std::stop_token stoken)
{
   ESP_LOGI(LOGTAG, "Process Message");
   auto m = new MCUMessage();
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
            CmdHandler->getQueue().push(m);
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
   uint8_t dataBuf[MCUMessage::MAX_MESSAGE_SIZE * 2] = { 0 };
   size_t received = 0;
   while (!stoken.stop_requested())
   {
#if !defined VIRTUAL_DEVICE
      receivePtr += uart_read_bytes(UART_NUM_1, &dataBuf[0], sizeof(dataBuf), 100);
#endif
      if (received > MCUMessage::ENVELOP_HEADER)
      {
         //ESP_LOG_BUFFER_HEXDUMP(LOGTAG, &dataBuf[0], received, ESP_LOG_INFO);
         int32_t consumed = processMessage(&dataBuf[0], received, stoken);
         if (consumed > 0)
         {
            ESP_LOGI(LOGTAG, "process Msg: consumed=%d total=%d", consumed, received);
            received -= consumed;
            assert(received < sizeof(dataBuf));
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

