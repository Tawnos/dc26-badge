#include "npc_interact.h"
#include "stm_to_esp_generated.h"
//#include "lib/System.h"
#include "mcu_to_mcu.h"

//#include "dc26.h"
#include <queue>
#include <mutex>
#include <chrono>
using namespace std::chrono_literals;

#define LOGTAG "NPCITTask"

void NPCInteractionTask::run(std::stop_token stoken)
{
   ESP_LOGI(LOGTAG, "NPCInteractionTask started");
   while (!stoken.stop_requested())
   {
      auto m = getQueue().pop();
      //if (xQueueReceive(getQueueHandle(), &m, 1000 / portTICK_PERIOD_MS))
      if (m)
      {
         if (m->RequestType == NPCMsg::NPCRequestType::Hello)
         {
            ESP_LOGI(LOGTAG, "got HELO");
            helo(m->MsgID);
         }
         else if (m->RequestType == NPCMsg::NPCRequestType::Interact)
         {
            ESP_LOGI(LOGTAG, "got Interact");
            interact(m);
         }
         delete m;
      }
   }
}
