#include "mcu_to_mcu.h"
#include "command_handler.h"
extern "C" {
#include "checksum.h"
}

#include <future>

#if !defined VIRTUAL_DEVICE
#include "dc26.h"
#include "dc26_ble/ble.h"
static uint8_t ESPToSTMBuffer[MCUToMCUTask::ESP_TO_STM_MSG_QUEUE_SIZE * MCUToMCUTask::ESP_TO_STM_MSG_ITEM_SIZE];
static StaticQueue_t OutgoingQueue;
static QueueHandle_t OutgoingQueueHandle = nullptr;
static xTaskHandle UARTSendTaskhandle = 0;
#endif
#include "npc_interact.h"


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

   auto m = new MCUMessage{ size, crc, msg };
   auto mPlaced = getQueue().push(m);
   assert(m != mPlaced);
   delete m;
}

int32_t MCUToMCUTask::processMessage(const uint8_t* data, uint32_t size)
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
            getQueue().push(m);
            //xQueueSend(CommandHandler->getQueueHandle(), (void*)&m, (TickType_t)0);
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
   auto commandHandler = [this](std::stop_token stoken, MCUMessage* m) {

      while (!stoken.stop_requested())
      {
         //if (xQueueReceive(getQueueHandle(), &m, (TickType_t)1000 / portTICK_PERIOD_MS))
         //auto m = getQueue().pop();
         if (m)
         {
            ESP_LOGI(LOGTAG, "got message from queue");
            const darknet7::STMToESPRequest* msg = m->asSTMToESP();
            ESP_LOGI(LOGTAG, "message type is: %d", msg->Msg_type());
            switch (msg->Msg_type())
            {
            case darknet7::STMToESPAny_SetupAP: {
#if !defined VIRTUAL_DEVICE
               const darknet7::SetupAP* sap = msg->Msg_as_SetupAP();
               wifi_auth_mode_t auth_mode = convertAuthMode(sap->mode());
               wifi.startAP(sap->ssid()->c_str(), sap->passwd()->c_str(), auth_mode);
               Port80WebServer.start(80, false);
#endif
            }
                                              break;
            case darknet7::STMToESPAny_StopAP: {
#if !defined VIRTUAL_DEVICE
               Port80WebServer.stop();
               wifi.stopWiFi();
               wifi.shutdown();
#endif
            }
                                             break;
            case darknet7::STMToESPAny_ESPRequest: {
               ESP_LOGI(LOGTAG, "processing esp system info request");
               flatbuffers::FlatBufferBuilder fbb;
               //System::logSystemInfo();
               //esp_chip_info_t chip;
               //System::getChipInfo(&chip);
               auto info = darknet7::CreateESPSystemInfoDirect(fbb,
                  //System::getFreeHeapSize(), System::getMinimumFreeHeapSize(),
                  1337, 31337,
                  //chip.model, chip.cores,
                  0xDC, 4,
                  //chip.revision, chip.features,
                  0xD0, 0,
                  //System::getIDFVersion()
                  0);
               flatbuffers::Offset<darknet7::ESPToSTM> of =
                  darknet7::CreateESPToSTM(fbb, msg->msgInstanceID(),
                     darknet7::ESPToSTMAny_ESPSystemInfo,
                     info.Union());
               darknet7::FinishSizePrefixedESPToSTMBuffer(fbb, of);
               send(fbb);
            }
                                                 break;
            case darknet7::STMToESPAny_CommunicationStatusRequest: {
               ESP_LOGI(LOGTAG, "processing communication status request");
#if !defined VIRTUAL_DEVICE
               flatbuffers::FlatBufferBuilder fbb;
               BluetoothTask& bttask = getBLETask();
               darknet7::WiFiStatus status = darknet7::WiFiStatus_DOWN;
               MyWiFiEventHandler* eh = (MyWiFiEventHandler*)wifi.getWifiEventHandler();
               if (eh && eh->isAPStarted())
               {
                  status = darknet7::WiFiStatus_AP;
               }
               auto s = darknet7::CreateCommunicationStatusResponseDirect(
                  fbb, status, bttask.advertising_enabled,
                  bttask.adv_name.c_str());
               flatbuffers::Offset<darknet7::ESPToSTM> of =
                  darknet7::CreateESPToSTM(fbb, msg->msgInstanceID(),
                     darknet7::ESPToSTMAny_CommunicationStatusResponse, s.Union());
               darknet7::FinishSizePrefixedESPToSTMBuffer(fbb, of);
               mcuToMcu->send(fbb);
#endif
            }
                                                                 break;
            case darknet7::STMToESPAny_WiFiScan: {
               ESP_LOGI(LOGTAG, "processing wifi scan request");
               display->getQueue().push(new	DisplayHandlerMessage("Scanning!", 0, 16, 2000));

#if !defined VIRTUAL_DEVICE
               MyWiFiEventHandler* eh = (MyWiFiEventHandler*)wifi.getWifiEventHandler();
               const darknet7::WiFiScan* ws = msg->Msg_as_WiFiScan();
               if (eh)
               {
                  eh->setScanMsgID(msg->msgInstanceID());
                  if (ws->filter() == darknet7::WiFiScanFilter_NPC)
                  {
                     eh->setWiFiScanNPCOnly(true);
                  }
                  //stop the wifi if its running
                  wifi.stopWiFi();
                  ESP_LOGI(LOGTAG, "starting scan requestID: %d", msg->msgInstanceID());
                  wifi.scan(false);
               }
#endif
            }
                                               break;
            case darknet7::STMToESPAny_WiFiNPCInteract: {
               ESP_LOGI(LOGTAG, "processing wifi npc interaction");
               //MyWiFiEventHandler *eh = (MyWiFiEventHandler*)wifi.getWifiEventHandler();
               const darknet7::WiFiNPCInteract* ws = msg->Msg_as_WiFiNPCInteract();
               auto result = 0;
#if !defined VIRTUAL_DEVICE
               wifi.stopWiFi();
               auto result = wifi.connectAP(ws->ssid(), "DCDN-7-DC26", ws->bssid()->data());
#endif
               if (result)
               {
                  NPCMsg* nmsg = 0;
                  if (ws->type() == 0)
                  { //LIST
                     ESP_LOGI(LOGTAG, "NPC List Request");
                     nmsg = new NPCMsg(NPCMsg::NPCRequestType::Hello, msg->msgInstanceID());
                  }
                  else
                  { //action
                     ESP_LOGI(LOGTAG, "NPC Interaction request");
                     if (ws->action() != 0)
                     {
                        nmsg = new NPCMsg(NPCMsg::NPCRequestType::Interact, msg->msgInstanceID(), ws->npcname()->c_str(), ws->action()->c_str());
                     }
                     else
                     {
                        nmsg = new NPCMsg(NPCMsg::NPCRequestType::Interact, msg->msgInstanceID(), ws->npcname()->c_str(), 0);
                     }
                  }
                  getDisplayQueue().push(nmsg);
                  delete nmsg;
#if !defined VIRTUAL_DEVICE
                  xQueueSend(npcInteractionTask.getQueueHandle(), (void*)&nmsg, (TickType_t)100);
#endif
               }
               else
               {
                  //let stm know we failed
                  flatbuffers::FlatBufferBuilder fbb;
                  auto s = darknet7::CreateNPCListDirect(fbb, nullptr, (int8_t)1);
                  flatbuffers::Offset<darknet7::ESPToSTM> of =
                     darknet7::CreateESPToSTM(fbb, msg->msgInstanceID(),
                        darknet7::ESPToSTMAny_NPCList, s.Union());
                  darknet7::FinishSizePrefixedESPToSTMBuffer(fbb, of);
                  send(fbb);
               }
            }
                                                      break;
            default:
               ESP_LOGI(LOGTAG, "Default case in command handler...");
               break;
            }
            delete m;
         }
      }
   };

   //esp_log_level_set(LOGTAG, ESP_LOG_INFO);
   uint8_t dataBuf[MCUMessage::MAX_MESSAGE_SIZE * 2] = { 0 };
   while (!stoken.stop_requested())
   {

      //      auto m = getQueue().pop();
      //      //if (xQueueReceive(OutgoingQueueHandle, &m, portMAX_DELAY))
      //      if (m)
      //      {
      //         uint32_t bytesSent = 0;
      //         ESP_LOGI(LOGTAG, "sending %d bytes of msg type %d with break!",
      //            (int)m->getMessageSize(), (int)m->asESPToSTM()->Msg_type());
      //#if !defined VIRTUAL_DEVICE
      //         bytesSent = uart_write_bytes_with_break(UART_NUM_1, m->getMessageData(), m->getMessageSize(), 16);
      //#endif
      //         if (m->getMessageSize() != bytesSent)
      //         {
      //            //ESP_LOGI(MCUToMCUTask::LOGTAG, "failed to send all bytes! %u of %u", bytesSent, m->getMessageSize());
      //         }
      //         else
      //         {
      //            //ESP_LOGI(MCUToMCUTask::LOGTAG, "Sent!");
      //         }
      //      }


#if !defined VIRTUAL_DEVICE
      receivePtr += uart_read_bytes(UART_NUM_1, &dataBuf[0], sizeof(dataBuf), 100);
#else
      auto receiveResult = uartReceiveBytes.wait_for(std::chrono::milliseconds{ 100 });
#endif

      if (receiveResult == std::future_status::ready)
      {
         auto result = uartReceiveBytes.get();
         auto received = result.size();

         assert(received < sizeof(dataBuf));
         while (received > MCUMessage::ENVELOP_HEADER)
         {
            //ESP_LOG_BUFFER_HEXDUMP(LOGTAG, &dataBuf[0], received, ESP_LOG_INFO);
            int32_t consumed = processMessage(&dataBuf[0], result.size());
            if (consumed > 0)
            {
               ESP_LOGI(LOGTAG, "process Message: consumed=%d total=%d", consumed, received);
               received -= consumed;
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
      }
      std::this_thread::sleep_for(50ms);
   }
}

