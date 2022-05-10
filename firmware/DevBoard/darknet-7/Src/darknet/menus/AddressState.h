#ifndef ADDRESS_STATE_H
#define ADDRESS_STATE_H

#include "darknet7_base_state.h"
#include "../contact.h"
#include <libstm32/display/gui.h>
class AddressState: public Darknet7BaseState {
public:	
	using Darknet7BaseState::Darknet7BaseState;
	virtual ~AddressState() = default;
	void resetSelection();
protected:
	virtual cmdc0de::ErrorType onInit() override;
	virtual Darknet7BaseState*  onRun() override;
	virtual cmdc0de::ErrorType onShutdown() override;
	void setNext4Items(uint16_t startAt);
private:
	cmdc0de::GUIListItemData Items[4];
	cmdc0de::GUIListData AddressList{ "Address Book", Items, 0, 0, cmdc0de::DISPLAY_WIDTH, cmdc0de::DISPLAY_HEIGHT, 0, sizeof(Items) / sizeof(Items[0]) };
	Contact CurrentContactList[4]{};
	cmdc0de::GUIListItemData DetailItems[5];
	cmdc0de::GUIListData ContactDetails{ "Contact Details: ", DetailItems, 0, 0, cmdc0de::DISPLAY_WIDTH, cmdc0de::DISPLAY_HEIGHT / 2, 0, sizeof(DetailItems) / sizeof(DetailItems[0]) };
	char RadioIDBuf[12]{ 0 };
	char PublicKey[64]{ 0 };
	char SignatureKey[128]{ 0 };
	uint8_t Index{ 0 };
	cmdc0de::GUIListData* DisplayList{ nullptr };
};


#endif
