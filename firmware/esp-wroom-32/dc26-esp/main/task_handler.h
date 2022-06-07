#pragma once
#include <thread>
class TaskHandler
{
	virtual void run(std::stop_token stoken) = 0;
};

#define ESP_LOGI(tag, string) printf("%s: %s", tag, string);