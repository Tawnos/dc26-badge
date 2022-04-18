#include "SendMsgState.h"
#include "../darknet7.h"

using cmdc0de::ErrorType;
using cmdc0de::RGBColor;
using cmdc0de::StateBase;

void SendMsgState::setContactToMessage(const uint16_t radioID, const char *agentName) {
	RadioID = radioID;
	AgentName = agentName;
}

ErrorType SendMsgState::onInit() {
	return ErrorType();
}

StateBase::ReturnStateContext SendMsgState::onRun() {
	StateBase *nextState = this;

	return ReturnStateContext(nextState);
}

ErrorType SendMsgState::onShutdown() {
	if (shouldReset()) {
		RadioID = 0;
		AgentName = 0;
	}
	return ErrorType();
}
