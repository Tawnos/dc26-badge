#ifndef DARKNET7_NPC_INTERACT
#define DARKNET7_NPC_INTERACT

//#include "esp_system.h"
//#include "esp_log.h"
//#include "lib/Task.h"
//#include "freertos/queue.h"
#include "mcu_to_mcu.h"
#include <stdint.h>
#include <string>
#include <chrono>
#include <array>
#include "command_queue.h"
#include "task_handler.h"
#include <flatbuffers/flatbuffers.h>
#include <cJSON.h>
#include "esp_to_stm_generated.h"


using namespace std::chrono_literals;

struct NPCMsg
{
   enum class NPCRequestType { None, Hello, Interact };

   NPCRequestType RequestType{ NPCRequestType::None };
   uint32_t MsgID{ 0 };
   char NpcName[32]{ 0 };
   char Action[32]{ 0 };

   NPCMsg() = default;
   NPCMsg(const NPCRequestType& r, uint32_t msgID) : RequestType(r), MsgID(msgID) {}
   NPCMsg(const NPCRequestType& r, uint32_t msgID, const char* name, const char* action)
      : RequestType(r), MsgID(msgID)
   {
      if (name)
      {
         strcpy(&NpcName[0], name);
      }
      else
      {
         memset(&NpcName[0], 0, sizeof(NpcName));
      }
      if (action)
      {
         strcpy(&Action[0], action);
      }
      else
      {
         memset(&Action[0], 0, sizeof(Action));
      }
   }
};

class NPCInteractionTask : public TaskHandler<NPCMsg, 10>
{
public:
   NPCInteractionTask(MCUToMCUTask* mcuToMcu) :mcuToMcu(mcuToMcu) {}
   virtual void run(std::stop_token stoken) override;
private:
   std::string HttpResponseStr;
   const char* TAG = "httpclient";
   MCUToMCUTask* mcuToMcu{ nullptr };

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
         /*ESP_LOGI(TAG, "Status = %d, content_length = %d",
            esp_http_client_get_status_code(client),
            esp_http_client_get_content_length(client));*/
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
      auto of = darknet7::CreateESPToSTM(fbb, msgId, darknet7::ESPToSTMAny_NPCList, s.Union());
      darknet7::FinishSizePrefixedESPToSTMBuffer(fbb, of);
      mcuToMcu->send(fbb);

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
#if !defined VIRTUAL_DEVICE
         ESP_LOGI(TAG, "Status = %d, content_length = %d",
            esp_http_client_get_status_code(client),
            esp_http_client_get_content_length(client));
#endif
         ESP_LOGI(TAG, "%s", HttpResponseStr.c_str());
         ESP_LOGI(TAG, "parsing json");
         root = cJSON_Parse(HttpResponseStr.c_str());
         cJSON* a = cJSON_GetObjectItem(root, "a");
         cJSON* d = cJSON_GetObjectItem(root, "d");
         cJSON* ji = cJSON_GetObjectItem(root, "i");
         cJSON* n = cJSON_GetObjectItem(root, "n");
         cJSON* r = cJSON_GetObjectItem(root, "r");
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
      mcuToMcu->send(fbb);

      if (root)
      {
         cJSON_Delete(root);
      }
#if !defined VIRTUAL_DEVICE
      esp_http_client_cleanup(client);
#endif
   }

   //InQueueHandle = xQueueCreateStatic(NPCMSG_QUEUE_SIZE, NPCMSG_ITEM_SIZE, &CommandBuffer[0], &npcMessageQueue);
   //if (InQueueHandle == nullptr)
   //{
   //   ESP_LOGI(LOGTAG, "Failed creating incoming queue");
   //}
   //uint8_t CommandBuffer[NPCMSG_QUEUE_SIZE * NPCMSG_ITEM_SIZE]{ 0 };
};

#endif
