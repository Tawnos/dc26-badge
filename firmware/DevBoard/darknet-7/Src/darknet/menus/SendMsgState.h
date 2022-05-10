#ifndef SEND_MSG_STATE_H
#define SEND_MSG_STATE_H

#include "darknet7_base_state.h"

class SendMsgState: public Darknet7BaseState {
public:
	static const uint16_t NO_CONTACT = 0xFFFF;
	enum INTERNAL_STATE {
		TYPE_MESSAGE, CONFIRM_SEND, SENDING
	};
	using Darknet7BaseState::Darknet7BaseState;
	virtual ~SendMsgState() = default;
	void setContactToMessage(const uint16_t radioID, const char *agentName);
protected:
	virtual cmdc0de::ErrorType onInit() override;
	virtual Darknet7BaseState*  onRun() override;
	virtual cmdc0de::ErrorType onShutdown() override;
private:
	uint16_t RadioID{ 0 };
	const char* AgentName{ 0 };
	char MsgBuffer[60]{ 0 };
	INTERNAL_STATE InternalState{ TYPE_MESSAGE };

};


#endif
