/*
 * darknet7_base_state.h
 *
 *  Created on: May 29, 2018
 *      Author: dcomes
 */
#pragma once
#ifndef DARKNET_MENUS_DARKNET7_BASE_STATE_H_
#define DARKNET_MENUS_DARKNET7_BASE_STATE_H_

#include "../virtual_key_board.h"
#include "../messaging/stm_to_esp_generated.h"
#include "../libstm32/display/gui.h"

 /*
  *	@author cmdc0de
  *	Usage:  Holds run time context information from the application, Current time thorugh the main loop, maybe some performance information
  *	Or anything else the library already knows about.
  */
class RunTimeContext
{
public:
private:
   uint32_t CurrentLoop;

};
class DarkNet7;
/*
 * @author: cmdc0de
 *
 * Usage:  Base class for the program state, any runtime information will need to be passed to the construction of the state
 * 	I did not want to template this class so I decided against passing some template parameter into the Run function.  Normally this will
 * 	hold references to the attached devices (LCD, Sensors, etc).
 */

class Darknet7BaseState
{
protected:
   DarkNet7* darknet;
   VirtualKeyBoard* VKB;

   static const uint32_t INIT_BIT = 0x01;
   static const uint32_t DONT_RESET = 0x02;
   static const uint32_t SHIFT_FROM_BASE = 8;
   virtual cmdc0de::ErrorType onInit() = 0;
   virtual Darknet7BaseState* onRun() = 0;
   virtual cmdc0de::ErrorType onShutdown() = 0;
   void setState(uint32_t n)
   {
      StateData |= n;
   }
   void clearState(uint32_t n)
   {
      StateData = (StateData & ~n);
   }
   bool checkState(uint32_t n)
   {
      return (StateData & n) != 0;
   }
   bool hasBeenInitialized()
   {
      return (StateData & INIT_BIT) != 0;
   }
   bool shouldReset()
   {
      return (StateData & DONT_RESET) == 0;
   }
   uint32_t getTimesRunCalledAllTime()
   {
      return TimesRunCalledAllTime;
   }
   uint32_t getTimesRunCalledSinceLastReset()
   {
      return TimesRunCalledSinceLastReset;
   }
   uint32_t timeInState()
   {
      return HAL_GetTick() - StateStartTime;
   }

public:
   Darknet7BaseState(DarkNet7* darknet);
   virtual ~Darknet7BaseState() { delete VKB; }
   Darknet7BaseState* run()
   {
      ++TimesRunCalledAllTime;
      Darknet7BaseState* sr(this);
      if (!hasBeenInitialized())
      {
         TimesRunCalledSinceLastReset = 0;
         cmdc0de::ErrorType et = init();
         if (!et.ok())
         {
            sr = nullptr;
         }
      }
      else
      {
         ++TimesRunCalledSinceLastReset;
         sr = onRun();
         if (sr != this)
         {
            shutdown();
         }
      }
      return sr;
   }

   cmdc0de::ErrorType init()
   {
      cmdc0de::ErrorType et = onInit();
      if (et.ok())
      {
         setState(INIT_BIT);
         StateStartTime = HAL_GetTick();
      }
      return et;
   }

   cmdc0de::ErrorType shutdown()
   {
      cmdc0de::ErrorType et = onShutdown();
      clearState(INIT_BIT);
      StateStartTime = 0;
      return et;
   }


private:
   uint32_t StateData : 8{0};
   uint32_t TimesRunCalledAllTime : 24{0};
   uint32_t TimesRunCalledSinceLastReset{ 0 };
   uint32_t StateStartTime{ 0 };
};

#endif /* DARKNET_MENUS_DARKNET7_BASE_STATE_H_ */
