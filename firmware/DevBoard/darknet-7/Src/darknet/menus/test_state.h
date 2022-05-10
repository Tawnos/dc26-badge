#ifndef EVENT_STATE_H
#define EVENT_STATE_H

#include "darknet7_base_state.h"

class TestState: public Darknet7BaseState {
public:
	static const uint16_t MAX_MSG_LEN = 64;
public:

public:
	using Darknet7BaseState::Darknet7BaseState;
	virtual ~TestState() = default;
protected:
	virtual cmdc0de::ErrorType onInit();
	virtual Darknet7BaseState*  onRun();
	virtual cmdc0de::ErrorType onShutdown();
private:
	static const uint32_t EXIT_COUNT = 20;
	cmdc0de::GUIListItemData Items[8];
	cmdc0de::GUIListData ButtonList{ "Button Info:", Items, 0, 0, cmdc0de::DISPLAY_WIDTH, cmdc0de::DISPLAY_HEIGHT, 0, sizeof(Items) / sizeof(Items[0])};
	char ListBuffer[8][24]{ 0 }; //height then width
	uint32_t TimesMidHasBeenHeld{ 0 };
};
#endif
