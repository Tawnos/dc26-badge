/*
 * setting_state.cpp
 *
 *  Created on: May 29, 2018
 *      Author: dcomes
 */

#include "setting_state.h"
#include <darknet7.h>
#if !defined VIRTUAL_DEVICE
#include "../messaging/stm_to_esp_generated.h"
#include "../messaging/esp_to_stm_generated.h"
#endif

using cmdc0de::ErrorType;

using cmdc0de::RGBColor;

ErrorType SettingState::onInit()
{
   SubState = 0;
   MiscCounter = 0;
   memset(&AgentName[0], 0, sizeof(AgentName));
   darknet->getGUI()->fillScreen(RGBColor::BLACK);
   darknet->getGUI()->drawList(&SettingList);
   IHC.set(&AgentName[0], sizeof(AgentName));
   return ErrorType();
}

static char Misc[8] = { '\0' };

Darknet7BaseState*  SettingState::onRun()
{
   Darknet7BaseState* nextState = this;
   auto buttonInfo = darknet->getButtonInfo();
   if (0 == SubState)
   {
      if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Up))
      {
         if (SettingList.selectedItem == 0)
         {
            SettingList.selectedItem = sizeof(Items) / sizeof(Items[0]) - 1;
         }
         else
         {
            SettingList.selectedItem--;
         }
      }
      else if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Down))
      {
         if (SettingList.selectedItem
            == (sizeof(Items) / sizeof(Items[0]) - 1))
         {
            SettingList.selectedItem = 0;
         }
         else
         {
            SettingList.selectedItem++;
         }
      }
      else if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Left))
      {
         SettingList.selectedItem = 0;
      }
      else if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Fire) || buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Mid))
      {
         SubState = SettingList.selectedItem + 100;
         darknet->getGUI()->fillScreen(RGBColor::BLACK);
         switch (SubState)
         {
         case 100:
            memset(&AgentName[0], 0, sizeof(AgentName));
            VKB->init(VirtualKeyBoard::STDKBNames, &IHC, 5, cmdc0de::DISPLAY_WIDTH - 5, 80, cmdc0de::RGBColor::WHITE, RGBColor::BLACK, RGBColor::BLUE, '_');
            darknet->getGUI()->drawString(0, 10, (const char*)"Current agent name:");
            if (*darknet->getContacts()->getSettings().getAgentName() == '\0')
            {
               darknet->getGUI()->drawString(0, 20, (const char*)"NOT SET");
            }
            else
            {
               darknet->getGUI()->drawString(0, 20, darknet->getContacts()->getSettings().getAgentName());
            }
            darknet->getGUI()->drawString(0, 40, (const char*)"MID button completes entry");
            darknet->getGUI()->drawString(0, 50, (const char*)"Set agent name:");
            darknet->getGUI()->drawString(0, 60, &AgentName[0]);
            break;
         case 101:
            MiscCounter = darknet->getContacts()->getSettings().getScreenSaverTime();
            break;
         case 102:
            darknet->getGUI()->drawString(0, 10, (const char*)"ERASE ALL\nCONTACTS?");
            darknet->getGUI()->drawString(0, 30, (const char*)"Fire1 = YES");
            break;
         }
      }
   }
   else
   {
      switch (SubState)
      {
      case 100:
         VKB->process();
         if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Mid) && AgentName[0] != '\0' && AgentName[0] != ' ' && AgentName[0] != '_')
         {
            AgentName[AGENT_NAME_LENGTH - 1] = '\0';
            if (darknet->getContacts()->getSettings().setAgentname(&AgentName[0]))
            {
               flatbuffers::FlatBufferBuilder fbb;
               auto r = darknet7::CreateBLESetDeviceNameDirect(fbb, darknet->getContacts()->getSettings().getAgentName());
               auto z = darknet7::CreateSTMToESPRequest(fbb, darknet->nextSeq(), darknet7::STMToESPAny_BLESetDeviceName, r.Union());
               darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, z);
               darknet->getMcuToMcu().send(fbb);
               nextState = darknet->getDisplayMessageState(darknet->getDisplayMenuState(), (const char*)"Save Successful", 2000);
            }
            else
            {
               nextState = darknet->getDisplayMessageState(darknet->getDisplayMenuState(), (const char*)"Save FAILED!", 4000);
            }
         }
         else
         {
            darknet->getGUI()->drawString(0, 60, &AgentName[0]);
         }
         break;
      case 101: {
         if (MiscCounter > 9) MiscCounter = 9;
         else if (MiscCounter < 1) MiscCounter = 1;
         sprintf(&Misc[0], "%d", (int)MiscCounter);
         darknet->getGUI()->drawString(0, 10, (const char*)"Badge Sleep Time:", RGBColor::WHITE, RGBColor::BLACK, 1, true);
         darknet->getGUI()->drawString(0, 30, (const char*)"Up to increase, down to decrease", RGBColor::WHITE, RGBColor::BLACK, 1, true);
         darknet->getGUI()->drawString(10, 60, &Misc[0], RGBColor::WHITE, RGBColor::BLACK, 1, true);
         darknet->getGUI()->drawString(0, 100, (const char*)"MID Button completes", RGBColor::WHITE, RGBColor::BLACK, 1, true);
         if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Up))
         {
            MiscCounter++;
         }
         else if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Down))
         {
            MiscCounter--;
         }
         else if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Mid))
         {
            if (darknet->getContacts()->getSettings().setScreenSaverTime(MiscCounter))
            {
               nextState = darknet->getDisplayMessageState(darknet->getDisplayMenuState(), (const char*)"Setting saved", 2000);
            }
            else
            {
               nextState = darknet->getDisplayMessageState(darknet->getDisplayMenuState(), (const char*)"Save FAILED!", 4000);
            }
         }
      }
              break;
      case 102:
         if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Fire))
         {
            darknet->getContacts()->resetToFactory();
            nextState = darknet->getDisplayMenuState();
         }
         else if (buttonInfo->wasAnyButtonReleased())
         {
            nextState = darknet->getDisplayMenuState();
         }
         break;
      }
   }
   if (SubState < 100 && buttonInfo->wasAnyButtonReleased())
   {
      darknet->getGUI()->drawList(&SettingList);
   }
   return nextState;
}

ErrorType SettingState::onShutdown()
{
   return ErrorType();
}
