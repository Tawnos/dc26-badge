/*
 * communications_settings.cpp
 *
 *  Created on: May 29, 2018
 *      Author: dcomes
 */
#include "../darknet7.h"
#include "communications_settings.h"
#include <messaging/stm_to_esp_generated.h>
#include <messaging/esp_to_stm_generated.h>
#include "gui_list_processor.h"
#include "menu_state.h"

using cmdc0de::ErrorType;
using cmdc0de::RGBColor;


class BLESetDeviceNameMenu: public Darknet7BaseState {
private:
	char NewDeviceName[13]{ 0 };
	VirtualKeyBoard::InputHandleContext IHC{ &NewDeviceName[0],sizeof(NewDeviceName) };
	const char* CurrentDeviceName{ nullptr };
	//uint32_t RequestID;
public:
	void setCurrentNamePtr(const char *p) {CurrentDeviceName = p;}
	using Darknet7BaseState::Darknet7BaseState;
	virtual ~BLESetDeviceNameMenu() = default;
protected:
	virtual cmdc0de::ErrorType onInit() {
		VKB->init(VirtualKeyBoard::STDKBLowerCase,&IHC,5, cmdc0de::DISPLAY_WIDTH-5,80,RGBColor::WHITE, RGBColor::BLACK,
				RGBColor::BLUE,'_');
		return ErrorType();
	}

	virtual Darknet7BaseState*  onRun() {
		Darknet7BaseState* nextState = this;
		auto buttonInfo = darknet->getButtonInfo();
		darknet->getGUI()->fillScreen(RGBColor::BLACK);

		VKB->process();

		darknet->getGUI()->drawString(0,10,(const char *)"Current Name: ");
		darknet->getGUI()->drawString(0,20, CurrentDeviceName);
		darknet->getGUI()->drawString(0,30, (const char *)"New Name:");
		darknet->getGUI()->drawString(0,40, &NewDeviceName[0]);
		darknet->getGUI()->drawString(0,60,(const char *)"Mid button finishes");
		if(buttonInfo->wereTheseButtonsReleased(ButtonPress::Mid)) {
			flatbuffers::FlatBufferBuilder fbb;
			auto r = darknet7::CreateBLESetDeviceNameDirect(fbb,&NewDeviceName[0]);
			auto z = darknet7::CreateSTMToESPRequest(fbb,darknet->nextSeq(),darknet7::STMToESPAny_BLESetDeviceName,r.Union());
			darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb,z);
			darknet->getMcuToMcu()->send(fbb);
			nextState = darknet->getCommunicationSettingState();
		}
		return nextState;
	}

	virtual cmdc0de::ErrorType onShutdown() {
		return ErrorType();
	}
};

class BLEBoolMenu: public Darknet7BaseState {
public:
	enum TYPE {AD};
private:
	cmdc0de::GUIListData BLEList;
	cmdc0de::GUIListItemData Items[2];
	const char ListBuffer[2][6]{ "Yes", "No" };
	bool YesOrNo{ false };
	TYPE Type;
public:
	BLEBoolMenu(DarkNet7* darknet, const char *name, TYPE t) 
	: Darknet7BaseState(darknet),
		BLEList(name, Items, 0, 0, cmdc0de::DISPLAY_WIDTH, cmdc0de::DISPLAY_HEIGHT, 0, sizeof(Items) / sizeof(Items[0])),
		Type(t) { }
	virtual ~BLEBoolMenu() {}
	void setValue(bool b) {YesOrNo = b;}
protected:
	virtual cmdc0de::ErrorType onInit() {
		BLEList.items[0].id = 0;
		BLEList.items[0].text = &ListBuffer[0][0];
		BLEList.items[1].id = 1;
		BLEList.items[1].text = &ListBuffer[1][0];
		if(YesOrNo) {
			BLEList.selectedItem = 0;
		} else {
			BLEList.selectedItem = 1;
		}
		return ErrorType();
	}

	virtual Darknet7BaseState*  onRun() {
		Darknet7BaseState* nextState = this;
		auto buttonInfo = darknet->getButtonInfo();
		darknet->getGUI()->fillScreen(RGBColor::BLACK);
		darknet->getGUI()->drawList(&BLEList);

		if (!GUIListProcessor::process(buttonInfo, &BLEList,(sizeof(Items) / sizeof(Items[0])))) {
			if(buttonInfo->wereTheseButtonsReleased(ButtonPress::Fire)) {
				flatbuffers::FlatBufferBuilder fbb;
				if(Type==AD) {
					auto r = darknet7::CreateBLEAdvertise(fbb,!BLEList.selectedItem);
					auto z = darknet7::CreateSTMToESPRequest(fbb,darknet->nextSeq(),darknet7::STMToESPAny_BLEAdvertise,r.Union());
					darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb,z);
				}
				darknet->getMcuToMcu()->send(fbb);
				HAL_Delay(1000);
				nextState = darknet->getCommunicationSettingState();
			}
		}
		return nextState;
	}

	virtual cmdc0de::ErrorType onShutdown() {
		return ErrorType();
	}
};


class WiFi: public Darknet7BaseState {
private:
	static const uint16_t NO_WORKING_TIME = 0xFFFF;

	char SSID[17]{ 0 };
	char Password[32]{ 0 };
	darknet7::WifiMode SecurityType{ darknet7::WifiMode_OPEN };
	VirtualKeyBoard::InputHandleContext IHC{ nullptr, 0 };
	cmdc0de::GUIListItemData Items[5];
	cmdc0de::GUIListData WifiSettingList{ "WiFi Settings:", Items, 0, 0, cmdc0de::DISPLAY_WIDTH, 70, 0, sizeof(Items) / sizeof(Items[0]) };
	char ListBuffer[5][33]{ 0 };
	uint16_t WorkingItem{ NO_WORKING_TIME };
	darknet7::WiFiStatus CurrentWiFiStatus{ darknet7::WiFiStatus_DOWN };
public:
	using Darknet7BaseState::Darknet7BaseState;
	virtual ~WiFi() = default;
	void setWifiStatus(darknet7::WiFiStatus c) {CurrentWiFiStatus = c;}
protected:
	virtual cmdc0de::ErrorType onInit() {
		memset(&SSID[0],0,sizeof(SSID));
		memset(&Password[0],0,sizeof(Password));
		memset(&ListBuffer[0],0,sizeof(ListBuffer));
		for (uint32_t i = 0; i < (sizeof(Items) / sizeof(Items[0])); i++) {
			Items[i].id = i;
			Items[i].text = &ListBuffer[i][0];
			Items[i].setShouldScroll();
		}
		WorkingItem = NO_WORKING_TIME;
		return ErrorType();
	}

	virtual Darknet7BaseState*  onRun() {
		Darknet7BaseState* nextState = this;
		auto buttonInfo = darknet->getButtonInfo();
		darknet->getGUI()->fillScreen(RGBColor::BLACK);
		sprintf(&ListBuffer[0][0],"AP Status: %s", darknet7::EnumNameWiFiStatus(CurrentWiFiStatus));
		if(CurrentWiFiStatus==darknet7::WiFiStatus_DOWN) {
			ListBuffer[1][0] = '\0';
			ListBuffer[2][0] = '\0';
			ListBuffer[3][0] = '\0';
		} else {
			sprintf(&ListBuffer[1][0],"AP Type: %s", darknet7::EnumNameWifiMode(SecurityType));
			sprintf(&ListBuffer[2][0],"SSID: %s", &SSID[0]);
			if(SecurityType==darknet7::WifiMode_OPEN ) {
				ListBuffer[3][0] = '\0';
			} else {
				sprintf(&ListBuffer[3][0],"Password: %s", &Password[0]);
			}
		}
		strcpy(&ListBuffer[4][0],"Submit");

		if(WorkingItem==NO_WORKING_TIME) {
			if (!GUIListProcessor::process(buttonInfo, &WifiSettingList,(sizeof(Items) / sizeof(Items[0])))) {
				if(buttonInfo->wereTheseButtonsReleased(ButtonPress::Fire)) {
					WorkingItem = WifiSettingList.selectedItem;

					switch(WorkingItem) {
					case 0:
						//do nothing
						break;
					case 1:
						break;
					case 2:
						IHC.set(&SSID[0],sizeof(SSID));
						VKB->init(VirtualKeyBoard::STDKBLowerCase,&IHC,5, cmdc0de::DISPLAY_WIDTH-5,100,RGBColor::WHITE,	RGBColor::BLACK, RGBColor::BLUE,'_');
						break;
					case 3:
						IHC.set(&Password[0],sizeof(Password));
						VKB->init(VirtualKeyBoard::STDKBLowerCase,&IHC,5, cmdc0de::DISPLAY_WIDTH-5,100,RGBColor::WHITE,	RGBColor::BLACK, RGBColor::BLUE,'_');
						break;
					case 4: {
							if(SSID[0]!='\0' || CurrentWiFiStatus==darknet7::WiFiStatus_DOWN) {
								flatbuffers::FlatBufferBuilder fbb;
								flatbuffers::Offset<void> msgOffset;
								darknet7::STMToESPAny Msg_type = darknet7::STMToESPAny_StopAP;
								if(CurrentWiFiStatus==darknet7::WiFiStatus_DOWN) {
									auto r = darknet7::CreateStopAP(fbb);
									msgOffset = r.Union();
								} else {
									auto r = darknet7::CreateSetupAPDirect(fbb,&SSID[0],&Password[0],SecurityType);
									msgOffset = r.Union();
									Msg_type = darknet7::STMToESPAny_SetupAP;
								}
								auto z = darknet7::CreateSTMToESPRequest(fbb,darknet->nextSeq(),Msg_type,msgOffset);
								darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb,z);
								darknet->getMcuToMcu()->send(fbb);
								nextState = darknet->getDisplayMessageState(darknet->getCommunicationSettingState(),(const char *)"Updating ESP",5000);
							} else {
								darknet->getGUI()->drawString(0,80,(const char *)"SID Can't be blank");
							}
						}
						break;
					}
				} else if ( buttonInfo->wereTheseButtonsReleased(ButtonPress::Mid)) {
					nextState = darknet->getCommunicationSettingState();
				}
			}
		} else {
			switch(WorkingItem) {
			case 0:
				if(buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Up) || buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Down)) {
					if(CurrentWiFiStatus==darknet7::WiFiStatus_DOWN) CurrentWiFiStatus = darknet7::WiFiStatus_AP_STA;
					else CurrentWiFiStatus = darknet7::WiFiStatus_DOWN;
				} else if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Mid) || buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Fire)) {
					WorkingItem = NO_WORKING_TIME;
				}
				break;
			case 1:
			{
				if(buttonInfo->wereTheseButtonsReleased(ButtonPress::Up)) {
					uint32_t s = (uint32_t)(SecurityType);
					++s;
					s = s%darknet7::WifiMode_MAX;
					SecurityType = (darknet7::WifiMode)s;
				} else if (buttonInfo->wereTheseButtonsReleased(ButtonPress::Down)) {
					if(SecurityType==darknet7::WifiMode_MIN) {
						SecurityType = darknet7::WifiMode_MAX;
					} else {
						uint32_t s = (uint32_t)(SecurityType);
						--s;
						SecurityType = (darknet7::WifiMode)s;
					}
				} else if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Mid) || buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Fire)) {
					WorkingItem = NO_WORKING_TIME;
				}
			}
			break;
			case 2:
				VKB->process();
				if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Mid)) {
					WorkingItem = NO_WORKING_TIME;
				}
				break;
			case 3:
				VKB->process();
				if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Mid)) {
					WorkingItem = NO_WORKING_TIME;
				}
				break;
			}
		}
		darknet->getGUI()->drawList(&WifiSettingList);
		return nextState;
	}

	virtual cmdc0de::ErrorType onShutdown() {
		WifiSettingList.selectedItem=0;
		return ErrorType();
	}
};

#if !defined VIRTUAL_DEVICE
static BLESetDeviceNameMenu BLESetName_Menu;
static BLEBoolMenu BLEAdvertise_Menu(nullptr, "BLE Advertise", BLEBoolMenu::AD);
static WiFi WiFiMenu;
#endif

void CommunicationSettingState::receiveSignal(MCUToMCU*,const MSGEvent<darknet7::CommunicationStatusResponse> *mevt) {
	if(mevt->RequestID==this->ESPRequestID) {
		for (uint32_t i = 0; i < (sizeof(Items) / sizeof(Items[0])); i++) {
			Items[i].text = &ListBuffer[i][0];
		}
		CurrentWifiStatus = mevt->InnerMsg->WifiStatus();
		sprintf(&ListBuffer[0][0], "Wifi Status: %s", EnumNameWiFiStatus(CurrentWifiStatus));
		sprintf(&ListBuffer[1][0], "BLE Advertise: %s", mevt->InnerMsg->BLEAdvertise()?cmdc0de::sYES: cmdc0de::sNO);
		sprintf(&ListBuffer[2][0], "BLE DeviceName: %s", mevt->InnerMsg->BLEDeviceName()->c_str());
		strcpy(&CurrentDeviceName[0],mevt->InnerMsg->BLEDeviceName()->c_str());
		darknet->getGUI()->fillScreen(RGBColor::BLACK);
		darknet->getGUI()->drawList(&CommSettingList);
		darknet->getMcuToMcu()->getBus().removeListener(this,mevt,darknet->getMcuToMcu());
		InternalState = DISPLAY_DATA;
	}
}

ErrorType CommunicationSettingState::onInit() {
	InternalState = FETCHING_DATA;
	flatbuffers::FlatBufferBuilder fbb;
	auto r = darknet7::CreateCommunicationStatusRequest(fbb);
	ESPRequestID = darknet->nextSeq();
	auto e = darknet7::CreateSTMToESPRequest(fbb,ESPRequestID,darknet7::STMToESPAny_CommunicationStatusRequest, r.Union());
	darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb,e);
	memset(&ListBuffer[0], 0, sizeof(ListBuffer));
	const MSGEvent<darknet7::CommunicationStatusResponse> *si = 0;
	darknet->getMcuToMcu()->getBus().addListener(this,si,darknet->getMcuToMcu());
	darknet->getGUI()->fillScreen(RGBColor::BLACK);
	darknet->getGUI()->drawString(5, 10, (const char*)"Fetching data from ESP");// , RGBColor::BLUE);
	darknet->getMcuToMcu()->send(fbb);
	return ErrorType();
}

Darknet7BaseState*  CommunicationSettingState::onRun() {
	Darknet7BaseState* nextState = this;
	auto buttonInfo = darknet->getButtonInfo();
	if(InternalState==FETCHING_DATA) {
		if(this->getTimesRunCalledSinceLastReset()>200) {
			const MSGEvent<darknet7::CommunicationStatusResponse> *mevt=0;
			darknet->getMcuToMcu()->getBus().removeListener(this,mevt,darknet->getMcuToMcu());
			nextState = darknet->getDisplayMessageState(darknet->getDisplayMenuState(),cmdc0de::NO_DATA_FROM_ESP,2000);
		}
	} else {
		if (!GUIListProcessor::process(buttonInfo, &CommSettingList,(sizeof(Items) / sizeof(Items[0])))) {
			if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Fire)) {
				darknet->getGUI()->fillScreen(RGBColor::BLACK);
#if !defined VIRTUAL_DEVICE
				switch(CommSettingList.selectedItem) {
				case 0:
					WiFiMenu.setWifiStatus(CurrentWifiStatus);
					nextState = &WiFiMenu;
					break;
				case 1:
					nextState = &BLEAdvertise_Menu;
					break;
				case 2:
					BLESetName_Menu.setCurrentNamePtr(&CurrentDeviceName[0]);
					nextState = &BLESetName_Menu;
					break;
				}
#else
				darknet->getGUI()->drawString(0, 0, "PROCESS MENU");
#endif
			} else if (buttonInfo->wereAnyOfTheseButtonsReleased(ButtonPress::Mid)) {
				nextState = darknet->getDisplayMenuState();
			}
		}
		darknet->getGUI()->drawList(&CommSettingList);
	}
	return nextState;
}

ErrorType CommunicationSettingState::onShutdown()
{
	return ErrorType();
}


