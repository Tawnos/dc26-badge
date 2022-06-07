#ifndef CMDHANDLER_H
#define CMDHANDLER_H

#if !defined VIRTUAL_DEVICE
#include "esp_system.h"
#include "esp_log.h"
#include "lib/Task.h"
#include "lib/wifi/WiFi.h"
#include "freertos/queue.h"
#endif

#include "mcu_to_mcu.h"
#include "command_queue.h"
#include "task_handler.h"
#include "npc_interact.h"
#include "display_handler.h"

class CmdHandlerTask : public TaskHandler
{
public:
   static const int STM_TO_ESP_MSG_QUEUE_SIZE = 4;
   static const int STM_TO_ESP_MSG_ITEM_SIZE = sizeof(MCUToMCUTask::Message*);
   static const char* LOGTAG;
public:
   CmdHandlerTask(const std::string& tName, uint16_t stackSize = 4196, uint8_t p = 5) {};
   //	
   //bool CmdHandlerTask::init()
   //{
   //   ESP_LOGI(LOGTAG, "INIT");
   //   IncomingQueueHandle = xQueueCreateStatic(STM_TO_ESP_MSG_QUEUE_SIZE,
   //      STM_TO_ESP_MSG_ITEM_SIZE, &CommandBuffer[0], &IncomingQueue);
   //   if (IncomingQueueHandle == nullptr)
   //   {
   //      ESP_LOGI(LOGTAG, "Failed creating incoming queue");
   //   }
   //   if (NPCITask.init())
   //   {
   //      NPCITask.start();
   //   }
   //   else
   //   {
   //      ESP_LOGE(LOGTAG, "NPC interact task not init'ed");
   //   }
   //   WiFiEventHandler* handler = new MyWiFiEventHandler();
   //   wifi.setWifiEventHandler(handler);
   //   //return wifi.init();
   //   return true;
   //}

public:
   virtual void run(std::stop_token stoken) override;
   constexpr CommandQueue<MCUToMCUTask::Message, 4>& getMessageQueue()
   {
      return messageQueue;
   }

protected:
   CommandQueue<MCUToMCUTask::Message, 4> messageQueue{};

#if !defined VIRTUAL_DEVICE
   WiFi wifi;
#endif
private:
   MCUToMCUTask* mcuToMcu{ nullptr };
   DisplayTask* display{ nullptr };
   NPCInteractionTask NPCITask{};// ("NPCInteractTask");
};

#endif
