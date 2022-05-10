#include "SendMsgState.h"
#include "../darknet7.h"

using cmdc0de::ErrorType;
using cmdc0de::RGBColor;


void SendMsgState::setContactToMessage(const uint16_t radioID, const char *agentName) {
	RadioID = radioID;
	AgentName = agentName;
}

ErrorType SendMsgState::onInit() {
	return ErrorType();
}

Darknet7BaseState*  SendMsgState::onRun() {
	Darknet7BaseState* nextState = this;

	return nextState;
}

ErrorType SendMsgState::onShutdown() {
	if (shouldReset()) {
		RadioID = 0;
		AgentName = 0;
	}
	return ErrorType();
}
