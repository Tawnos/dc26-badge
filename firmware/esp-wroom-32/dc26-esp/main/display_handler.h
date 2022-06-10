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

class DisplayDevice {
public:
	void Fill(uint8_t color) {}
	void GotoXY(uint8_t x, uint8_t y) {}
	void Puts(const char* s, const FontDef_t* font, uint8_t color) {}
	void UpdateScreen() {}

};

class DisplayHandlerTask : public TaskHandler<DisplayMsg, 6>
{
public:

	//	bool DisplayHandlerTask::init()
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
	virtual void run(std::stop_token stoken) override;
private:
	DisplayDevice* display{ nullptr };
};

#endif
