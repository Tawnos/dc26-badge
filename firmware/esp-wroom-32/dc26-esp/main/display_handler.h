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

class DisplayTask : public TaskHandler
{
public:
	struct DisplayMsg
	{
		char Msg[30]{0};
		uint8_t x{0}, y{16};
		uint16_t TimeInMSToDisplay{2000};
		DisplayMsg() = default;
		DisplayMsg(const char *msg, uint8_t x1, uint8_t y1, uint16_t ms)
			: x(x1), y(y1), TimeInMSToDisplay(ms)
		{
			strncpy(&Msg[0], msg, sizeof(Msg));
			// strcpy(&Msg[0],msg);
		}
	};
	static const int DISPLAY_QUEUE_SIZE = 6;
	static const int DISPLAY_MSG_ITEM_SIZE = sizeof(DisplayTask::DisplayMsg *);
	static const char *LOGTAG;

public:

	//	bool DisplayTask::init()
	//{
	//	ESP_LOGI(LOGTAG, "INIT");
	//	if (SSD1306_Init(&I2cDisplay) > 0)
	//	{
	//		ESP_LOGI(LOGTAG, "display init successful");
	//		SSD1306_Puts("BOOTING...", &Font_11x18, SSD1306_COLOR_WHITE);
	//		SSD1306_UpdateScreen();
	//	}
	//	else
	//	{
	//		ESP_LOGI(LOGTAG, "display init UN-successful");
	//	}
	//	IncomingQueueHandle = xQueueCreateStatic(DISPLAY_QUEUE_SIZE,
	//		DISPLAY_MSG_ITEM_SIZE, &CommandBuffer[0], &IncomingQueue);
	//	if (IncomingQueueHandle == nullptr)
	//	{
	//		ESP_LOGI(LOGTAG, "Failed creating incoming queue");
	//	}
	//	return true;
	//}
	bool init();
	void showMessage(const char *m, uint8_t x1, uint8_t y1, uint16_t ms);

public:
	virtual void run(std::stop_token stoken) override;

protected:
	CommandQueue<DisplayMsg, 6> displayQueue{};

private:
};

#endif
