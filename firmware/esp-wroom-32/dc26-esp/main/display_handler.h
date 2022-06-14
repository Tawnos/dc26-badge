#ifndef DISPLAYHANDLER_H
#define DISPLAYHANDLER_H

#if !defined VIRTUAL_DEVICE
#include "esp_system.h"
#include "esp_log.h"
#include "lib/Task.h"
#include "freertos/queue.h"
#include "lib/wifi/WiFi.h"
#endif

#include "mcu_to_mcu.h"
#include <string.h>
#include <thread>
#include "task_handler.h"
#include "command_queue.h"
#include <fonts/fonts.h>
#include "display_device.h"


struct DisplayMsg
{
	char Msg[30]{ 0 };
	uint8_t x{ 0 }, y{ 16 };
	uint16_t TimeInMSToDisplay{ 2000 };
	DisplayMsg() = default;
	DisplayMsg(const char* msg, uint8_t x1, uint8_t y1, uint16_t ms)
		: x(x1), y(y1), TimeInMSToDisplay(ms)
	{
		strncpy(&Msg[0], msg, sizeof(Msg));
		// strcpy(&Msg[0],msg);
	}
};


class DisplayHandlerTask : public TaskHandler<DisplayMsg, 6>
{
public:
	DisplayHandlerTask(DisplayDevice* display)
		: display(display) {}
	virtual void run(std::stop_token stoken) override;
private:
	DisplayDevice* display{ nullptr };
};

#endif
