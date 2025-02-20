#include "AddressState.h"
#include "SendMsgState.h"
#include "../darknet7.h"
#include "menu_state.h"
#include "gui_list_processor.h"

using cmdc0de::ErrorType;
using cmdc0de::RGBColor;

////////////////////////////////////////////////
ErrorType AddressState::onInit() {
	setNext4Items(0);
	for (uint16_t i = 0; i < sizeof(DetailItems) / sizeof(DetailItems[0]); ++i) {
		DetailItems[i].id = 0;
		DetailItems[i].Scrollable = 0;
	}
	memset(&RadioIDBuf[0], 0, sizeof(RadioIDBuf));
	memset(&PublicKey[0], 0, sizeof(PublicKey));
	memset(&SignatureKey[0], 0, sizeof(SignatureKey));
	darknet->getGUI()->fillScreen(RGBColor::BLACK);
	DisplayList = &AddressList;
	darknet->getGUI()->drawList(DisplayList);
	Index = 0;
	return ErrorType();
}

void AddressState::resetSelection() {
	AddressList.selectedItem = 0;
}

void AddressState::setNext4Items(uint16_t startAt) {
	uint8_t num = darknet->getContactStore()->getSettings().getNumContacts();
	for (uint16_t i = startAt, j = 0; j < (4); i++, j++) {
		if (i < num) {
			if (darknet->getContactStore()->getContactAt(i, CurrentContactList[j])) {
				Items[j].id = CurrentContactList[j].getUniqueID();
				Items[j].text = CurrentContactList[j].getAgentName();
			}
		} else {
			Items[j].id = 0;
			Items[j].text = "";
		}
	}
}

Darknet7BaseState*  AddressState::onRun() {
	Darknet7BaseState *nextState = this;
	auto buttonInfo = darknet->getButtonInfo();
	if (DetailItems[0].id == 0) {
		if(buttonInfo->wereTheseButtonsReleased(ButtonPress::Up)) {
			if (AddressList.selectedItem == 0) {
				//keep selection at 0 but load new values
				int16_t startAt = Index - 4;
				if (startAt > 0) {
					Index = startAt;
					setNext4Items(startAt);
				} else {
					setNext4Items(0);
					Index = 0;
					AddressList.selectedItem = 0;
				}
			} else {
				AddressList.selectedItem--;
				Index--;
			}
		} else if (buttonInfo->wereTheseButtonsReleased(ButtonPress::Down)) {
			uint16_t num = darknet->getContactStore()->getSettings().getNumContacts();
			if (Index < num) {
				if (AddressList.selectedItem == (sizeof(Items) / sizeof(Items[0]) - 1)) {
					if (num > Index + 4) {
						setNext4Items(Index);
					} else {
						Index = num - 2;
						setNext4Items(Index);
					}
					AddressList.selectedItem = 0;
				} else {
					AddressList.selectedItem++;
					Index++;
				}
			}
		} else if(buttonInfo->wereTheseButtonsReleased(ButtonPress::Mid)) {
			nextState = darknet->getDisplayMenuState();
		} else if (buttonInfo->wereTheseButtonsReleased(ButtonPress::Fire)) {
			if (Items[AddressList.selectedItem].id != 0) {
				DisplayList = &ContactDetails;
				DetailItems[0].id = 1;
				DetailItems[0].text = CurrentContactList[AddressList.selectedItem].getAgentName();
				DetailItems[1].id = 1;
				sprintf(&RadioIDBuf[0], "ID: %d", CurrentContactList[AddressList.selectedItem].getUniqueID());
				DetailItems[1].text = &RadioIDBuf[0];
				DetailItems[2].id = 1;
				uint8_t *pk = CurrentContactList[AddressList.selectedItem].getCompressedPublicKey();
				memset(&PublicKey[0], 0, sizeof(PublicKey));
				sprintf(&PublicKey[0],
						"PK: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
						pk[0], pk[1], pk[2], pk[3], pk[4], pk[5], pk[6], pk[7], pk[8], pk[9], pk[10], pk[11],
						pk[12], pk[13], pk[14], pk[15], pk[16], pk[17], pk[18], pk[19], pk[20], pk[21], pk[22],
						pk[23], pk[24]);
				DetailItems[2].text = &PublicKey[0];
				DetailItems[2].resetScrollable();
				DetailItems[3].id = 1;
				uint8_t *sig = CurrentContactList[AddressList.selectedItem].getPairingSignature();
				memset(&SignatureKey[0], 0, sizeof(SignatureKey));
				sprintf(&SignatureKey[0],
					"PAIR SIG: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
					sig[0], sig[1], sig[2], sig[3], sig[4], sig[5], sig[6], sig[7], sig[8], sig[9], sig[10],
					sig[11], sig[12], sig[13], sig[14], sig[15], sig[16], sig[17], sig[18], sig[19],sig[20],
					sig[21], sig[22], sig[23], sig[24], sig[25], sig[26], sig[27], sig[28], sig[29],sig[30],
					sig[31], sig[32], sig[33], sig[34], sig[35], sig[36], sig[37], sig[38], sig[39],sig[40],
					sig[41], sig[42], sig[43], sig[44], sig[45], sig[46], sig[47]);
				DetailItems[3].text = &SignatureKey[0];
				DetailItems[3].resetScrollable();
			}
		}
	} else {
		if(!GUIListProcessor::process(buttonInfo, DisplayList,DisplayList->ItemsCount)) {
			if(buttonInfo->wereTheseButtonsReleased(ButtonPress::Mid)) {
				DetailItems[0].id = 0;
				darknet->getGUI()->fillScreen(RGBColor::BLACK);
				DisplayList = &AddressList;
			}
		}
	}
	darknet->getGUI()->drawList(DisplayList);
	return nextState;
}

ErrorType AddressState::onShutdown() {
	return ErrorType();
}
