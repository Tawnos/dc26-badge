/*
 * badge_info_state.h
 *
 *  Created on: May 29, 2018
 *      Author: dcomes
 */

#ifndef DARKNET_MENUS_BADGE_INFO_STATE_H_
#define DARKNET_MENUS_BADGE_INFO_STATE_H_

#include "darknet7_base_state.h"
#include <libstm32/display/gui.h>

class ContactStore;

class BadgeInfoState: public Darknet7BaseState {
public:
	using Darknet7BaseState::Darknet7BaseState;
	virtual ~BadgeInfoState() = default;
protected:
	virtual cmdc0de::ErrorType onInit() override;
	virtual Darknet7BaseState*  onRun() override;
	virtual cmdc0de::ErrorType onShutdown() override;
	const char *getRegCode(ContactStore *cs);
private:
	cmdc0de::GUIListItemData Items[9]{};
	cmdc0de::GUIListData BadgeInfoList{ "Badge Info:", Items, 0, 0, cmdc0de::DISPLAY_WIDTH, cmdc0de::DISPLAY_HEIGHT, 0, sizeof(Items) / sizeof(Items[0]) };
	char ListBuffer[9][32]{ 0 }; //height then width
	char RegCode[18]{ 0 };
};


#endif /* DARKNET_MENUS_BADGE_INFO_STATE_H_ */
