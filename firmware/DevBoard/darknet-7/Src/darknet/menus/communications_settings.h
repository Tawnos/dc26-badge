/*
 * communications_settings.h
 *
 *  Created on: May 29, 2018
 *      Author: dcomes
 */

#ifndef DARKNET_MENUS_COMMUNICATIONS_SETTINGS_H_
#define DARKNET_MENUS_COMMUNICATIONS_SETTINGS_H_

#include "darknet7_base_state.h"
#include "../mcu_to_mcu.h"
#include "../virtual_key_board.h"
#include <libstm32/display/gui.h>


namespace darknet7 {
	class ESPToSTM;
	class CommunicationStatusResponse;
}


class CommunicationSettingState: public Darknet7BaseState {
public:
	using Darknet7BaseState::Darknet7BaseState;
	virtual ~CommunicationSettingState() = default;

	void receiveSignal(MCUToMCU*,const MSGEvent<darknet7::CommunicationStatusResponse> *mevt);
protected:
	enum INTERNAL_STATE { NONE, FETCHING_DATA, DISPLAY_DATA };
	virtual cmdc0de::ErrorType onInit() override;
	virtual Darknet7BaseState*  onRun() override;
	virtual cmdc0de::ErrorType onShutdown() override;
private:
	cmdc0de::GUIListItemData Items[3]{};
	cmdc0de::GUIListData CommSettingList{ "Comm Info:", Items, 0, 0, cmdc0de::DISPLAY_WIDTH, cmdc0de::DISPLAY_HEIGHT, 0, sizeof(Items) / sizeof(Items[0]) };
	char ListBuffer[3][24]{ 0 };
	char CurrentDeviceName[13]{ 0 };
	uint32_t ESPRequestID{ 0 };
	INTERNAL_STATE InternalState{ NONE };
	darknet7::WiFiStatus CurrentWifiStatus{ darknet7::WiFiStatus_DOWN };
};



#endif /* DARKNET_MENUS_COMMUNICATIONS_SETTINGS_H_ */
