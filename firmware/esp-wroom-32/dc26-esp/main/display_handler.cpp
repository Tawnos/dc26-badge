#include "display_handler.h"
#include "lib/System.h"
#include "lib/i2c.hpp"
#include "lib/ssd1306.h"
#include "lib/fonts.h"
#include "dc26.h"

const char *DisplayTask::LOGTAG = "DisplayTask";
static StaticQueue_t IncomingQueue;
static uint8_t CommandBuffer[DisplayTask::DISPLAY_QUEUE_SIZE 
	* DisplayTask::DISPLAY_MSG_ITEM_SIZE] = { 0 };

ESP32_I2CMaster I2cDisplay(GPIO_NUM_19,GPIO_NUM_18,1000000, I2C_NUM_0, 0, 32);

DisplayTask::DisplayTask(const std::string &tName, uint16_t stackSize, uint8_t p) :
		Task(tName, stackSize, p), IncomingQueueHandle(nullptr) {

}

bool DisplayTask::init() {
	ESP_LOGI(LOGTAG, "INIT");
	if(SSD1306_Init(&I2cDisplay)>0) {
		ESP_LOGI(LOGTAG,"display init successful");
		SSD1306_Puts("BOOTING...", &Font_11x18, SSD1306_COLOR_WHITE);
		SSD1306_UpdateScreen();
	} else {
		ESP_LOGI(LOGTAG,"display init UN-successful");
	}
	IncomingQueueHandle = xQueueCreateStatic(DISPLAY_QUEUE_SIZE,
			DISPLAY_MSG_ITEM_SIZE, &CommandBuffer[0], &IncomingQueue);
	if (IncomingQueueHandle == nullptr) {
		ESP_LOGI(LOGTAG, "Failed creating incoming queue");
	}
	return true;
}

DisplayTask::~DisplayTask() {
}

void DisplayTask::showMessage(const char *m, uint8_t x1, uint8_t y1, uint16_t ms) {
	DisplayMsg *dm = new	DisplayMsg(m, x1, y1,  ms);
	xQueueSend(getQueueHandle(), (void* )&dm,(TickType_t ) 100);
}

#define RANDOM_MSG 21 
const char *RandomMessages[RANDOM_MSG] =  {
	  "WHO U?"
	, "Who dat?"
	, "Be safe!"
	, "Never know.."
	, "Con crud..."
	, "who dat"
	, "Gourry"
	, "0a's ponies?"
	, "PLAGUE!"
	, "AAAACHEW!"
	, "excuse me..."
	, "Chicken?"
	, "RAT?????"
	, "cmdc0de"
	, "sanity"
	, "wild?"
	, "mild?"
	, "bunni"
	, "skyria"
	, "silky soft"
	, "mansel"
};

void DisplayTask::run(void *data) {
	ESP_LOGI(LOGTAG, "Display Task started");
	SSD1306_Fill(SSD1306_COLOR_BLACK);
	SSD1306_GotoXY(0,8);
	SSD1306_Puts(" Welcome ", &Font_11x18, SSD1306_COLOR_WHITE);
	SSD1306_GotoXY(0,40);
	SSD1306_Puts("Darknet 7", &Font_11x18, SSD1306_COLOR_WHITE);
	SSD1306_UpdateScreen();
	DisplayTask::DisplayMsg *m;
	while (1) {
		if (xQueueReceive(getQueueHandle(), &m, (TickType_t) 1000 / portTICK_PERIOD_MS)) {
			ESP_LOGI(LOGTAG, "got message from queue");
			SSD1306_Fill(SSD1306_COLOR_BLACK);
			SSD1306_GotoXY(m->x,m->y);
			SSD1306_Puts(&m->Msg[0], &Font_11x18, SSD1306_COLOR_WHITE);
			SSD1306_UpdateScreen();
			uint32_t time = m->TimeInMSToDisplay;
			delete m;
			vTaskDelay((TickType_t) time/portTICK_PERIOD_MS);
		} else {
			static uint32_t timeOfLastDisplay = 0;
			uint32_t now = (xTaskGetTickCount() / (portTICK_RATE_MS ));
			//ESP_LOGI(LOGTAG, "NOw = %u, last = %u",now, timeOfLastDisplay);
			if((now-timeOfLastDisplay)>100) {
				int chance = rand()%100;
				//ESP_LOGI(LOGTAG, "chance %d",chance);
				if(chance<20) {
					//ESP_LOGI(LOGTAG,"New random message");
					int msg = rand()%RANDOM_MSG;
					SSD1306_Fill(SSD1306_COLOR_BLACK);
					SSD1306_GotoXY(0,32);
					SSD1306_Puts(RandomMessages[msg], &Font_11x18, SSD1306_COLOR_WHITE);
					SSD1306_UpdateScreen();
				}
			  timeOfLastDisplay = now;	
			}
		}
	}
}

