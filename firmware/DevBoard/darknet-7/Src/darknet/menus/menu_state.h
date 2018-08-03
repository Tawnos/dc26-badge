#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "darknet7_base_state.h"
#include "../libstm32/etl/src/random.h"
#include "../mcu_to_mcu.h"
#include "../messaging/stm_to_esp_generated.h"
#include "../messaging/esp_to_stm_generated.h"

class MenuState: public Darknet7BaseState {
public:
	MenuState();
	virtual ~MenuState();
	void receiveSignal(MCUToMCU*, const MSGEvent<darknet7::BLEInfectionData>* mevt);
protected:
	virtual cmdc0de::ErrorType onInit();
	virtual cmdc0de::StateBase::ReturnStateContext onRun();
	virtual cmdc0de::ErrorType onShutdown();
private:
	cmdc0de::GUIListData MenuList;
	cmdc0de::GUIListItemData Items[12];
	etl::random_xorshift rand;
};


#endif
