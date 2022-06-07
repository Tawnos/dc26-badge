#include "display_handler.h"
#include <chrono>
#include <thread>
using namespace std::chrono_literals;

#if !defined VIRTUAL_DEVICE
#include "lib/System.h"
#include "lib/i2c.hpp"
#include "lib/ssd1306.h"
#include "lib/fonts.h"
#include "dc26.h"

const char* DisplayTask::LOGTAG = "DisplayTask";
static StaticQueue_t IncomingQueue;
static uint8_t CommandBuffer[DisplayTask::DISPLAY_QUEUE_SIZE * DisplayTask::DISPLAY_MSG_ITEM_SIZE] = { 0 };

ESP32_I2CMaster I2cDisplay(GPIO_NUM_19, GPIO_NUM_18, 1000000, I2C_NUM_0, 0, 32);
#endif

void DisplayTask::showMessage(const char* m, uint8_t x1, uint8_t y1, uint16_t ms)
{
   DisplayMsg* dm = new	DisplayMsg(m, x1, y1, ms);
   //xQueueSend(getQueueHandle(), (void*)&dm, (TickType_t)100);
}

#define RANDOM_MSG 21 
const char* RandomMessages[RANDOM_MSG] = {
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

void DisplayTask::run(std::stop_token stoken)
{
#if !defined VIRTUAL_DEVICE
   ESP_LOGI(LOGTAG, "Display Task started");
   SSD1306_Fill(SSD1306_COLOR_BLACK);
   SSD1306_GotoXY(0, 8);
   SSD1306_Puts(" Welcome ", &Font_11x18, SSD1306_COLOR_WHITE);
   SSD1306_GotoXY(0, 40);
   SSD1306_Puts("Darknet 7", &Font_11x18, SSD1306_COLOR_WHITE);
   SSD1306_UpdateScreen();
#endif
   while (!stoken.stop_requested())
   {
      auto m = displayQueue.pop(stoken);
      //if (xQueueReceive(getQueueHandle(), &m, (TickType_t)1000 / portTICK_PERIOD_MS))
      if (m)
      {
#if !defined VIRTUAL_DEVICE
         ESP_LOGI(LOGTAG, "got message from queue");
         SSD1306_Fill(SSD1306_COLOR_BLACK);
         SSD1306_GotoXY(m->x, m->y);
         SSD1306_Puts(&m->Msg[0], &Font_11x18, SSD1306_COLOR_WHITE);
         SSD1306_UpdateScreen();
#endif
         uint32_t time = m->TimeInMSToDisplay;
         delete m;
         std::this_thread::sleep_for(std::chrono::milliseconds{ time });
      }
      else
      {
         static auto timeOfLastDisplay = std::chrono::steady_clock::now();
         auto now = std::chrono::steady_clock::now();
         //ESP_LOGI(LOGTAG, "NOw = %u, last = %u",now, timeOfLastDisplay);
         if (std::chrono::duration_cast<std::chrono::seconds>(now - timeOfLastDisplay) > 100s)
         {
            int chance = rand() % 100;
            //ESP_LOGI(LOGTAG, "chance %d",chance);
            if (chance < 20)
            {
               //ESP_LOGI(LOGTAG,"New random message");
               int msg = rand() % RANDOM_MSG;
#if !defined VIRTUAL_DEVICE
               SSD1306_Fill(SSD1306_COLOR_BLACK);
               SSD1306_GotoXY(0, 32);
               SSD1306_Puts(RandomMessages[msg], &Font_11x18, SSD1306_COLOR_WHITE);
               SSD1306_UpdateScreen();
#else
               printf("%s", RandomMessages[msg]);
#endif
            }
            timeOfLastDisplay = now;
         }
      }
   }
}

