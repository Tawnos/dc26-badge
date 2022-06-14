/*
 * health.cpp
 *
 *  Created on: May 30, 2018
 *      Author: cmdc0de
 */



#include "health.h"
#include <darknet7.h>
#include <error_type.h>

ErrorType Health::onInit()
{
   memset(&ListBuffer[0], 0, sizeof(ListBuffer));
   sprintf(&ListBuffer[0][0], "Avian Flu    : %s", darknet->getContactStore()->getSettings().isInfectedWith(SettingsInfo::AVIAN_FLU) ? cmdc0de::sYES : cmdc0de::sNO);
   sprintf(&ListBuffer[1][0], "Measles      : %s", darknet->getContactStore()->getSettings().isInfectedWith(SettingsInfo::MEASLES) ? cmdc0de::sYES : cmdc0de::sNO);
   sprintf(&ListBuffer[2][0], "Tetanus      : %s", darknet->getContactStore()->getSettings().isInfectedWith(SettingsInfo::TETANUS) ? cmdc0de::sYES : cmdc0de::sNO);
   sprintf(&ListBuffer[3][0], "Polio        : %s", darknet->getContactStore()->getSettings().isInfectedWith(SettingsInfo::POLIO) ? cmdc0de::sYES : cmdc0de::sNO);
   sprintf(&ListBuffer[4][0], "Plague       : %s", darknet->getContactStore()->getSettings().isInfectedWith(SettingsInfo::PLAGUE) ? cmdc0de::sYES : cmdc0de::sNO);
   sprintf(&ListBuffer[5][0], "Toxoplasmosis: %s", darknet->getContactStore()->getSettings().isInfectedWith(SettingsInfo::TOXOPLASMOSIS) ? cmdc0de::sYES : cmdc0de::sNO);
   sprintf(&ListBuffer[6][0], "Chlamydia    : %s", darknet->getContactStore()->getSettings().isInfectedWith(SettingsInfo::CHLAMYDIA) ? cmdc0de::sYES : cmdc0de::sNO);
   sprintf(&ListBuffer[7][0], "Herpes       : %s", darknet->getContactStore()->getSettings().isInfectedWith(SettingsInfo::HERPES) ? cmdc0de::sYES : cmdc0de::sNO);

   for (uint32_t i = 0; i < (sizeof(Items) / sizeof(Items[0])); i++)
   {
      Items[i].text = &ListBuffer[i][0];
      Items[i].id = 1 << (i + 1);
      Items[i].setShouldScroll();
   }
   darknet->getGUI()->fillScreen(RGBColor::BLACK);
   darknet->getGUI()->drawList(&HealthList);
   return ErrorType();

}

Darknet7BaseState*  Health::onRun()
{
   Darknet7BaseState* nextState = this;
   auto buttonInfo = darknet->getButtonInfo();
   if (!GUIListProcessor::process(buttonInfo, &HealthList, (sizeof(Items) / sizeof(Items[0]))))
   {
      if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Mid))
      {
         nextState = darknet->getDisplayMenuState();
      }
      else if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Fire) &&
         darknet->getContactStore()->getSettings().isInfectedWith(HealthList.items[HealthList.selectedItem].id))
      {
         EnterCure.setVirus(HealthList.selectedItem);
         nextState = &EnterCure;
      }
   }
   darknet->getGUI()->drawList(&HealthList);
   return nextState;
}

ErrorType Health::onShutdown()
{
   return ErrorType();
}

cmdc0de::ErrorType CureEntry::onInit()
{
   memset(&FinalHexHash[0], 0, sizeof(FinalHexHash));
   memset(&CureSequence[0], 0, sizeof(CureSequence));
   IHC.set(&CureSequence[0], sizeof(CureSequence));
   VKB->init(VirtualKeyBoard::STDCAPS, &IHC, 5, cmdc0de::DISPLAY_WIDTH - 5, 100, RGBColor::WHITE, RGBColor::BLACK, RGBColor::BLUE, '_');
   InternalState = VIRUS_ENTRY;
   return ErrorType();
}

Darknet7BaseState*  CureEntry::onRun()
{
   Darknet7BaseState* nextState = this;
   auto buttonInfo = darknet->getButtonInfo();
   auto gui = darknet->getGUI();
   gui->fillScreen(RGBColor::BLACK);
   if (InternalState == VIRUS_ENTRY)
   {
      VKB->process();

      gui->drawString(0, 10, (const char*)"Enter Sequence press:");
      gui->drawString(0, 20, &CureSequence[0]);
      gui->drawString(0, 60, (const char*)"Press MID to commit cure");
      if (buttonInfo->wereTheseButtonsReleased(ButtonPress::Mid))
      {
         if (validateCure())
         {
            InternalState = DISPLAY_QUEST_KEY;
            uint8_t mhash[SHA256_HASH_SIZE] = { 0 };
            ShaOBJ HCtx;
            sha256_init(&HCtx);
            sha256_add(&HCtx, darknet->getContactStore()->getMyInfo().getPublicKey(), PUBLIC_KEY_LENGTH);
            sha256_add(&HCtx, (const unsigned char*)&CureCodes[CurrentVirus], strlen((const char*)&CureCodes[CurrentVirus]));
            sha256_digest(&HCtx, &mhash[0]);
            sprintf((char*)&FinalHexHash[0], "%02x%02x%02x%02x%02x%02x%02x%02x", mhash[0], mhash[1], mhash[2], mhash[3], mhash[4], mhash[5], mhash[6], mhash[7]);
            //+1 bc cure all is 0 bit
            darknet->getContactStore()->getSettings().cure(1 << (CurrentVirus + 1));
         }
         else
         {
            nextState = darknet->getDisplayMessageState(this, (const char*)"Invalid Cure Code", 1000);
         }
      }
   }
   else if (InternalState == DISPLAY_QUEST_KEY)
   {
      char buf[24];
      sprintf(&buf[0], "of %s", VirusNames[CurrentVirus]);
      gui->drawString(0, 10, "Your badge has been cured");
      gui->drawString(0, 20, &buf[0]);
      gui->drawString(0, 40, "Send To Daemon: ");
      gui->drawString(0, 50, (const char*)&FinalHexHash[0]);
      if (buttonInfo->wereTheseButtonsReleased(ButtonPress::Mid))
      {
         nextState = darknet->getHealthState();
      }
   }
   return nextState;
}
