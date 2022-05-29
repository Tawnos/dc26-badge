#ifndef DARKNET7_NPC_INTERACT
#define DARKNET7_NPC_INTERACT

//#include "esp_system.h"
//#include "esp_log.h"
//#include "lib/Task.h"
//#include "freertos/queue.h"
#include "mcu_to_mcu.h"
#include <stdint.h>
#include <string>
#include <queue>

typedef void* QueueHandle_t;

struct NPCMsg
{
	enum class NPCRequestType { None, Hello, Interact };
	NPCRequestType RType{ NPCRequestType::None };
	uint32_t MsgID{ 0 };
	char NpcName[32]{ 0 };
	char Action[32]{ 0 };

	NPCMsg(const NPCRequestType& r, uint32_t msgID) : RType(r), MsgID(msgID) {}
	NPCMsg(const NPCRequestType& r, uint32_t msgID, const char* name, const char* action)
		: RType(r), MsgID(msgID)
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

class NPCInteractionTask : public Task {
public:
	static const int NPCMSG_QUEUE_SIZE = 10;
	static const int NPCMSG_ITEM_SIZE = sizeof(NPCMsg);
	static const char *LOGTAG;
public:
	bool init();
	virtual void run(void *data);
	virtual ~NPCInteractionTask() = default;
private:
	uint8_t CommandBuffer[NPCMSG_QUEUE_SIZE * NPCMSG_ITEM_SIZE]{ 0 };
	std::queue<NPCMsg> npcMessageQueue;// { CommandBuffer };
}; 

#endif
