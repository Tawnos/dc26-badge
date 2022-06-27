#pragma once
#ifndef IRMENU_H_
#define IRMENU_H_

#include "darknet7_base_state.h"
#include "contact.h"
#include "../mcu_to_mcu.h"
#include <libstm32/display/gui.h>

namespace darknet7 {
	class ESPToSTM;
}

class PairingState: public Darknet7BaseState {
public:
	struct AliceInitConvo {
		uint8_t irmsgid;
		uint8_t AlicePublicKey[PUBLIC_KEY_COMPRESSED_LENGTH];
		uint16_t AliceRadioID;
		char AliceName[AGENT_NAME_LENGTH];
	};

	struct BobReplyToInit {
		uint8_t irmsgid;
		uint8_t BoBPublicKey[PUBLIC_KEY_COMPRESSED_LENGTH];
		uint16_t BoBRadioID;
		char BobAgentName[AGENT_NAME_LENGTH];
		uint8_t SignatureOfAliceData[SIGNATURE_LENGTH];
	};

	struct AliceToBobSignature {
		uint8_t irmsgid;
		uint8_t signature[48];
	};
public:
	PairingState(DarkNet7* darknet);

	virtual ~PairingState() = default;
	virtual cmdc0de::ErrorType onInit() override;
	virtual Darknet7BaseState* onRun() override;
	virtual cmdc0de::ErrorType onShutdown() override;

	void receiveSignal(MCUToMCU*,const MSGEvent<darknet7::BadgesInArea>* mevt);
	void receiveSignal(MCUToMCU*,const MSGEvent<darknet7::BLESecurityConfirm>* mevt);
	void receiveSignal(MCUToMCU*,const MSGEvent<darknet7::BLEConnected>* mevt);
	void receiveSignal(MCUToMCU*,const MSGEvent<darknet7::BLEMessageFromDevice>* mevt);
	void receiveSignal(MCUToMCU*,const MSGEvent<darknet7::BLEPairingComplete>* mevt);
protected:
	enum INTERNAL_STATE { NONE, FETCHING_DATA, DISPLAY_DATA,
							INITIATING_CONNECTION, CONNECTING, CONFIRMING,
							ALICE_SEND_ONE, ALICE_SEND_TWO,
							BOB_SEND_ONE, BOB_SEND_TWO,
							RECEIVE_DATA,
							PAIRING_SUCCESS, PAIRING_COMPLETE, PAIRING_FAILED };
	void CleanUp();
private:
	// Badge:Address list
	cmdc0de::GUIListData BadgeList{ "Badge List:", Items, 0, 0, 160, 128, 0, (sizeof(Items) / sizeof(Items[0])) };
	cmdc0de::GUIListItemData Items[8]{};
	char ListBuffer[8][12]{ 0 };
	char AddressBuffer[8][18]{ 0 };
	char MesgBuf[200];
	unsigned int MesgLen{ 0 };

	// Internal State information
	INTERNAL_STATE InternalState{ NONE };
	uint32_t ESPRequestID{ 0 };
	uint32_t timesRunCalledSinceReset{ 0 };

	// Pairing State information
	uint16_t TimeoutMS{ 1000 };
	uint8_t RetryCount{ 3 };
	uint8_t CurrentRetryCount{ 0 };
	uint32_t TimeInState{ 0 };
	AliceInitConvo AIC{};
	BobReplyToInit BRTI{};
	AliceToBobSignature ATBS{};
	unsigned char msgId = 1;
	bool gotBadgeList = false;
	bool securityConfirmed = false;
	bool bleConnected = false;
	bool isAlice = false;
};

#endif

