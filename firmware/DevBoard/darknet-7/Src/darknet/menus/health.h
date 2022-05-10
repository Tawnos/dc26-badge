/*
 * tamagotchi.h
 *
 *  Created on: May 30, 2018
 *      Author: dcomes
 */

#ifndef DARKNET_MENUS_HEALTH_H_
#define DARKNET_MENUS_HEALTH_H_

#include "darknet7_base_state.h"
#include "../virtual_key_board.h"
#include "../libstm32/crypto/sha256.h"
#include "menu_state.h"
#include "gui_list_processor.h"
#include <libstm32/display/gui.h>

using cmdc0de::RGBColor;
using cmdc0de::ErrorType;

static const char* VirusNames[] = {
      "Avian Flu"
      ,"Measles"
      , "Tetanus"
      , "Polio"
      , "Plague"
      , "Toxoplasmosis"
      , "Chlamydia"
      , "Herpes"
};

static const char* CureCodes[] = {
        "AGTAGAAACAAGG"
      , "GTCAGTTCCACAT"
      , "GAGGTGCAGCTGG"
      , "ATTCTAACCATGG"
      , "AAGAGTATAATCG"
      , "CCTAAACCCTGAA"
      , "GTATTAGTATTTG"
      , "GATCGTTATTCCC"
};

class CureEntry : public Darknet7BaseState
{
public:
   enum INTERNAL_STATE { NONE, VIRUS_ENTRY, DISPLAY_QUEST_KEY };
private:
   char CureSequence[14]{ 0 };
   VirtualKeyBoard::InputHandleContext IHC{ &CureSequence[0], sizeof(CureSequence) };
   INTERNAL_STATE InternalState{ NONE };
   uint8_t CurrentVirus{ 0 };
   uint8_t FinalHexHash[SHA256_HASH_SIZE]{ 0 };
public:
   using Darknet7BaseState::Darknet7BaseState;
   virtual ~CureEntry() = default;
   void setVirus(uint8_t v) { CurrentVirus = v; }
protected:
   bool validateCure()
   {
      return !strcmp(&CureSequence[0], CureCodes[CurrentVirus]);
   }

   virtual cmdc0de::ErrorType onInit() override;
   virtual Darknet7BaseState*  onRun() override;
   virtual cmdc0de::ErrorType onShutdown() override
   {
      return ErrorType();
   }
};


class Health: public Darknet7BaseState {
public:
   using Darknet7BaseState::Darknet7BaseState;
	virtual ~Health() = default;
protected:
	virtual cmdc0de::ErrorType onInit() override;
	virtual Darknet7BaseState*  onRun() override;
	virtual cmdc0de::ErrorType onShutdown() override;
private:
   cmdc0de::GUIListItemData Items[8]{};
   cmdc0de::GUIListData HealthList{ "Infections:", Items, 0, 0, cmdc0de::DISPLAY_WIDTH, cmdc0de::DISPLAY_HEIGHT, 0, sizeof(Items) / sizeof(Items[0]) };
   char ListBuffer[8][24]{ 0 };

   CureEntry EnterCure = CureEntry{ darknet };
};




#endif /* DARKNET_MENUS_TAMAGOTCHI_H_ */
