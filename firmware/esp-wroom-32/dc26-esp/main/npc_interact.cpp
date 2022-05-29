#include "npc_interact.h"
#include "stm_to_esp_generated.h"
#include "esp_to_stm_generated.h"
//#include "lib/System.h"
#include "mcu_to_mcu.h"
#include <cJSON.h>
#include "dc26.h"
#include <queue>

#define ESP_LOGI(tag, string) printf("%s %s", tag, string);
const char* TAG = "httpclient";
std::string HttpResponseStr;

//static StaticQueue_t npcMessageQueue;
const char* NPCInteractionTask::LOGTAG = "NPCITTask";

bool NPCInteractionTask::init()
{
   //npcMessageQueue.
   //InQueueHandle = xQueueCreateStatic(NPCMSG_QUEUE_SIZE, NPCMSG_ITEM_SIZE, &CommandBuffer[0], &npcMessageQueue);
   //if (InQueueHandle == nullptr)
   //{
   //   ESP_LOGI(LOGTAG, "Failed creating incoming queue");
   //}
   return true;
}

void helo(uint32_t msgId)
{
   HttpResponseStr.clear();
   uint32_t err = 0;
#if !defined VIRTUAL_DEVICE
   esp_http_client_config_t config;
   memset(&config, 0, sizeof(config));
   config.url = "http://192.168.4.1:8080/npc";
   config.event_handler = _http_event_handle;

   esp_http_client_handle_t client = esp_http_client_init(&config);
   err = esp_http_client_perform(client);
#endif
   flatbuffers::FlatBufferBuilder fbb;
   uint8_t wasError = 0;
   std::vector<flatbuffers::Offset<flatbuffers::String>> npcnames;
   cJSON* root = 0;
   if (!err)
   {
      ESP_LOGI(TAG, "Status = %d, content_length = %d",
         esp_http_client_get_status_code(client),
         esp_http_client_get_content_length(client));
      ESP_LOGI(TAG, "%s", HttpResponseStr.c_str());
      ESP_LOGI(TAG, "parsing json");
      root = cJSON_Parse(HttpResponseStr.c_str());
      if (cJSON_IsArray(root->child))
      {
         ESP_LOGI(TAG, "child is array");
         int size = cJSON_GetArraySize(root->child);
         for (int i = 0; i < size; i++)
         {
            cJSON* item = cJSON_GetArrayItem(root->child, i);
            ESP_LOGI(TAG, "item %d, %s", i, cJSON_GetStringValue(item));
            auto n = fbb.CreateString(cJSON_GetStringValue(item), strlen(cJSON_GetStringValue(item)));
            npcnames.push_back(n);
         }
      }
      else
      {
         wasError = 1;
      }
   }
   else
   {
      wasError = 1;
   }
   auto s = darknet7::CreateNPCListDirect(fbb, &npcnames, wasError);
   flatbuffers::Offset<darknet7::ESPToSTM> of =
      darknet7::CreateESPToSTM(fbb, msgId, darknet7::ESPToSTMAny_NPCList, s.Union());
   darknet7::FinishSizePrefixedESPToSTMBuffer(fbb, of);
   getMCUToMCU().send(fbb);

   if (root) cJSON_Delete(root);
#if !defined VIRTUAL_DEVICE
   esp_http_client_cleanup(client);
#endif
}

void interact(NPCMsg* m)
{
   HttpResponseStr.clear();
#if !defined VIRTUAL_DEVICE
   esp_http_client_config_t config;
#endif
   std::string url = "http://192.168.4.1:8080/npc/";
   url += m->NpcName;
   if (m->Action[0] != '\0')
   {
      url.append("/");
      url.append(m->Action);
   }

   uint32_t err = 0;
#if !defined VIRTUAL_DEVICE
   memset(&config, 0, sizeof(config));
   config.url = url.c_str();
   config.event_handler = _http_event_handle;

   esp_http_client_handle_t client = esp_http_client_init(&config);
   err = esp_http_client_perform(client);
#endif
   flatbuffers::FlatBufferBuilder fbb;
   std::vector<flatbuffers::Offset<flatbuffers::String>> actions;
   uint8_t wasError = 0;
   char* name = 0;
   char* desc = 0;
   uint32_t infections = 0;
   cJSON* root = 0;
   char* resp = 0;
   if (!err)
   {
      ESP_LOGI(TAG, "Status = %d, content_length = %d",
         esp_http_client_get_status_code(client),
         esp_http_client_get_content_length(client));
      ESP_LOGI(TAG, "%s", HttpResponseStr.c_str());
      ESP_LOGI(TAG, "parising json");
      root = cJSON_Parse(HttpResponseStr.c_str());
      cJSON* a = cJSON_GetObjectItem(root, (const char*)"a");
      cJSON* d = cJSON_GetObjectItem(root, (const char*)"d");
      cJSON* ji = cJSON_GetObjectItem(root, (const char*)"i");
      cJSON* n = cJSON_GetObjectItem(root, (const char*)"n");
      cJSON* r = cJSON_GetObjectItem(root, (const char*)"r");
      if (a && d && ji && n && cJSON_IsArray(a) && cJSON_IsArray(ji))
      {
         ESP_LOGI(TAG, "actions and infects is an arrary");
         int size = cJSON_GetArraySize(a);
         for (int i = 0; i < size; i++)
         {
            cJSON* item = cJSON_GetArrayItem(a, i);
            ESP_LOGI(TAG, "item %d, %s", i, cJSON_GetStringValue(item));
            auto n = fbb.CreateString(cJSON_GetStringValue(item), strlen(cJSON_GetStringValue(item)));
            actions.push_back(n);
         }
         size = cJSON_GetArraySize(ji);
         for (int i = 0; i < size; i++)
         {
            infections |= cJSON_GetArrayItem(ji, i)->valueint;
         }
         name = cJSON_GetStringValue(n);
         desc = cJSON_GetStringValue(d);
         resp = cJSON_GetStringValue(r);
      }
      else
      {
         wasError = 1;
      }
   }
   else
   {
      wasError = 1;
   }
   auto s = darknet7::CreateNPCInteractionResponseDirect(fbb, name, desc, &actions, (uint16_t)infections, resp, wasError);
   flatbuffers::Offset<darknet7::ESPToSTM> of =
      darknet7::CreateESPToSTM(fbb, m->MsgID, darknet7::ESPToSTMAny_NPCInteractionResponse, s.Union());
   darknet7::FinishSizePrefixedESPToSTMBuffer(fbb, of);
   getMCUToMCU().send(fbb);

   if (root) cJSON_Delete(root);
#if !defined VIRTUAL_DEVICE
   esp_http_client_cleanup(client);
#endif
}


void NPCInteractionTask::run(void* data)
{
   ESP_LOGI(LOGTAG, "NPCInteractionTask started");
   while (1)
   {
      NPCMsg* m = 0;
      //if (xQueueReceive(getQueueHandle(), &m, 1000 / portTICK_PERIOD_MS))
      if (!npcMessageQueue.empty())
      {
         auto message = npcMessageQueue.front();
         if (m->RType == NPCMsg::NPCRequestType::Hello)
         {
            ESP_LOGI(LOGTAG, "got HELO");
            helo(m->MsgID);
         }
         else if (m->RType == NPCMsg::NPCRequestType::Interact)
         {
            ESP_LOGI(LOGTAG, "got Interact");
            interact(m);
         }
         delete m;
      }
   }
}
