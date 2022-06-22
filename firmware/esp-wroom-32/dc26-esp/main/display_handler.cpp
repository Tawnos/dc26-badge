#include "display_handler.h"
#include "display_device.h"
//#include "lib/ssd1306.h"
#include <chrono>
#include <thread>
using namespace std::chrono_literals;

#if !defined VIRTUAL_DEVICE
#include "lib/System.h"
#include "lib/i2c.hpp"
#include "dc26.h"

static StaticQueue_t IncomingQueue;
static uint8_t CommandBuffer[DisplayTask::DISPLAY_QUEUE_SIZE * DisplayTask::DISPLAY_MSG_ITEM_SIZE] = { 0 };

ESP32_I2CMaster I2cDisplay(GPIO_NUM_19, GPIO_NUM_18, 1000000, I2C_NUM_0, 0, 32);
#endif

#define LOGTAG "DisplayTask"

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

void DisplayHandlerTask::run(std::stop_token stoken)
{
   ESP_LOGI(LOGTAG, "Display Task started");
   display->Fill(0);
   display->GotoXY(0, 8);  
   display->Puts(" Welcome ");
   display->GotoXY(0, 39);
   display->Puts("Darknet 7");
   display->UpdateScreen();
   while (!stoken.stop_requested())
   {
      if (auto m = getQueue().pop())
      {
         ESP_LOGI(LOGTAG, "got message from queue");
         display->Fill(0);
         display->GotoXY(m->x, m->y);
         display->Puts(&m->Msg[0]);
         display->UpdateScreen();
         uint32_t time = m->TimeInMSToDisplay;
         delete m;
         std::this_thread::sleep_for(std::chrono::milliseconds{ time });
      }
      else
      {
         static auto timeOfLastDisplay = std::chrono::steady_clock::now();
         auto now = std::chrono::steady_clock::now();
         //ESP_LOGI(LOGTAG, "NOw = %u, last = %u",now, timeOfLastDisplay);
         if (std::chrono::duration_cast<std::chrono::seconds>(now - timeOfLastDisplay) > 30s)
         {
            int msg = rand() % RANDOM_MSG;
            display->Fill(0);
            display->GotoXY(0, 32);
            display->Puts(RandomMessages[msg]);
            display->UpdateScreen();
            timeOfLastDisplay = now;
         }
      }
   }
}

