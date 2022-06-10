#pragma once
#include <thread>
#include "command_queue.h"

template <class TMessage, std::size_t NQueueSlots>
class TaskHandler
{
public:
	virtual void run(std::stop_token stoken) = 0;
	constexpr CommandQueue<TMessage, NQueueSlots>& getQueue()
	{
		return queue;
	}
private:
	CommandQueue<TMessage, NQueueSlots> queue{};
};

#define ESP_LOGI(tag, string) printf("%s: %s", tag, string);