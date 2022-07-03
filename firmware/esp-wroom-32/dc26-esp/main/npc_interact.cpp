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

void NPCInteractionTask::handleTask(const NPCMsg* message)
{
   if (message->RequestType == NPCMsg::NPCRequestType::Hello)
   {
      ESP_LOGI(LOGTAG, "got HELO");
      helo(message->MsgID);
   }
   else if (message->RequestType == NPCMsg::NPCRequestType::Interact)
   {
      ESP_LOGI(LOGTAG, "got Interact");
      interact(message);
   }
}
