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

//template <template<typename TMessage>... TTaskHandlers>//, typename TTaskHandlers = std::common_type<Args...>>
class CommandHandler
{
public:
   //CommandHandler(DisplayHandler) {}

   //const TMessage*... Push(TMessage... message) {  }
   //TMessage*... Pop() { }

   //constexpr CommandQueue<TMessage, NQueueSlots>& getQueue() {}
   //void 
   //CmdHandlerTask(const std::string& tName, uint16_t stackSize = 4196, uint8_t p = 5) {};
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
   //   if (npcInteractionTask.init())
   //   {
   //      npcInteractionTask.start();
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
   //virtual void run(std::stop_token stoken) override;

protected:

#if !defined VIRTUAL_DEVICE
   WiFi wifi;
#endif
private:

   //MCUToMCUTask mcuTask{this};
   //DisplayHandlerTask* displayTask;
   //NPCInteractionTask npcInteractionTask{ &mcuTask };
   //std::jthread npcInteractionTaskRunner{ std::bind_front(&NPCInteractionTask::run, &npcInteractionTask) };
};

#endif
