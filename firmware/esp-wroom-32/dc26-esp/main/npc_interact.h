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

class NPCInteractionTask : public TaskHandler {
public:
	virtual void run(std::stop_token stoken) override;
	auto& getMessageQueue()
	{
		return npcMessageQueue;
	}
private:
	CommandQueue<NPCMsg, 10> npcMessageQueue{1000ms};
	//std::array<NPCMsg, 10> commandBuffer{ };
	//InQueueHandle = xQueueCreateStatic(NPCMSG_QUEUE_SIZE, NPCMSG_ITEM_SIZE, &CommandBuffer[0], &npcMessageQueue);
	//if (InQueueHandle == nullptr)
	//{
	//   ESP_LOGI(LOGTAG, "Failed creating incoming queue");
	//}
	//uint8_t CommandBuffer[NPCMSG_QUEUE_SIZE * NPCMSG_ITEM_SIZE]{ 0 };
}; 

#endif
