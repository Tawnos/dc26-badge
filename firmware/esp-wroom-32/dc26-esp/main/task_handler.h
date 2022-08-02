#pragma once
#include <thread>
#include "command_queue.h"
#include "command_handler.h"


template <typename TTaskHandler>
class TaskRunner {
public:
	void run(std::stop_token stoken)
	{
		auto taskHandler = static_cast<TTaskHandler*>(this);
		while (!stoken.stop_requested())
		{
			auto nextMessage = taskHandler->pop();
			if (nextMessage)
			{
				taskHandler->handleTask(nextMessage);
				delete nextMessage;
			}
		}
	}
};

template <typename TMessageIn, template <typename> class TTaskHandler, typename... TMessagesOut>
class TaskHandlerBase : TaskRunner<TaskHandlerBase<TMessageIn>>
{
protected:
	void handleTask(std::unique_ptr<const TMessageIn> message) {
		auto taskHandler = static_cast<TTaskHandler<TMessageIn, TMessagesOut...>*>(this);
		taskHandler->handleTask(message);
	}

   std::unique_ptr<const TMessagesOut>... send(std::unique_ptr<const TMessagesOut>... message) {
		//std::unique_ptr<const TMessagesOut>...{};
		return nullptr;
	}
	
	std::unique_ptr<TMessageIn> pop() { 
		auto taskHandler = static_cast<TTaskHandler*>(this);
		return std::unique_ptr<TMessageIn>{taskHandler->getQueue().pop()};
	}
};

template <typename TTaskHandler, typename TMessageIn>
class TaskHandler : public TaskHandlerBase<TMessageIn, TaskHandler<TMessageIn>>
{
protected:
	void handleTask(std::unique_ptr<const TMessageIn> message)
	{
		auto taskHandler = static_cast<TTaskHandler*>(this);
		taskHandler->handleTask(message);
	}

	constexpr CommandQueue<TMessageIn, NQueueSlots>& getQueue()
	{
		return queue;
	}
	CommandQueue<TMessageIn, NQueueSlots> queue{};
};

template <typename... TTaskHandlers>
class TaskHandlerCollection {
	TaskHandlerCollection(TTaskHandlers... taskHandlers) : taskHandlers(taskHandlers) {}
	//(const Mixins&... mixins) : Mixins(mixins)... {}
	std::tuple<TTaskHandlers...> taskHandlers;
};

#define ESP_LOGI(tag, stringf) printf("%s: %s", tag, stringf);