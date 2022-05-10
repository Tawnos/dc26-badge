#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "darknet7_base_state.h"
#include <error_type.h>
#include "../mcu_to_mcu.h"
#include "../messaging/stm_to_esp_generated.h"
#include "../messaging/esp_to_stm_generated.h"
#include <libstm32/display/gui.h>

class MenuState : public Darknet7BaseState
{
public:
   using Darknet7BaseState::Darknet7BaseState;
   virtual ~MenuState() = default;

protected:
   virtual cmdc0de::ErrorType onInit() override;
   virtual Darknet7BaseState*  onRun() override;
   virtual cmdc0de::ErrorType onShutdown() override;
private:
   cmdc0de::GUIListItemData Items[12];
   cmdc0de::GUIListData MenuList{ "Main Menu", Items, 0, 0, cmdc0de::DISPLAY_WIDTH,cmdc0de::DISPLAY_HEIGHT, 0, sizeof(Items) / sizeof(Items[0]) };
};


#endif
