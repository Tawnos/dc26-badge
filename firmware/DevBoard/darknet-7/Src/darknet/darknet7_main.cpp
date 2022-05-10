//#include "ws2812b.h"
#include "darknet7.h"
#include "libstm32/display/display_st7735.h"
#include <main.h>

#include <usart.h>
#include <spi.h>
#include <i2c.h>
#include <ff.h>

using cmdc0de::DisplayST7735;

ErrorType DarkNet7::onInit() {

	ErrorType et;

	GUIListItemData items[3];
	GUIListData DrawList((const char*)"Self Check", items, uint8_t(0),
		uint8_t(0), uint8_t(DISPLAY_WIDTH), uint8_t(DISPLAY_HEIGHT / 2), uint8_t(0), uint8_t(0));
	//DO SELF CHECK
	if ((et = Display.init(DisplayST7735::SixteenBit, &Font_6x10, &DisplayBuffer)).ok()) {
		items[0].set(0, "OLED_INIT");
		DrawList.ItemsCount++;
		Display.setTextColor(cmdc0de::RGBColor::WHITE);
	}
	else {
		while (1) {
			HAL_GPIO_WritePin(SIMPLE_LED1_GPIO_Port, SIMPLE_LED1_Pin, GPIO_PIN_SET);
			HAL_Delay(200);
			HAL_GPIO_WritePin(SIMPLE_LED1_GPIO_Port, SIMPLE_LED1_Pin, GPIO_PIN_RESET);
			HAL_Delay(400);
		}
	}
	GUI gui(&Display);
	gui.drawList(&DrawList);
	Display.swap();
	HAL_Delay(500);
	if (MyContacts.getMyInfo().init()) {
		items[1].set(1, "Flash INIT");
	}
	else {
		items[1].set(1, "Flash INIT FAILED!");
	}
	DrawList.ItemsCount++;
	DrawList.selectedItem++;
	gui.drawList(&DrawList);
	Display.swap();
	HAL_Delay(500);
	MyContacts.getSettings().init();
	Display.fillScreen(cmdc0de::RGBColor::BLACK);
	Display.swap();
	Display.drawImage(32, 16, getLogo1());
	Display.swap();
	HAL_Delay(1000);
	Display.drawImage(32, 16, getLogo2());
	Display.swap();
	HAL_Delay(1000);
	Display.drawImage(32, 16, getLogo3());
	Display.swap();
	HAL_Delay(1000);



	//HAL_GPIO_WritePin(GPIO_APA106_DATA_GPIO_Port,GPIO_APA106_DATA_Pin,GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(GPIO_APA106_DATA_GPIO_Port,GPIO_APA106_DATA_Pin,GPIO_PIN_SET);

#if DEBUG_WHY_CANT_CHANGE_ROTATION
	//Display.setRotation(cmdc0de::DisplayDevice::LandscapeTopLeft,true);
	Display.fillScreen(cmdc0de::RGBColor::BLACK);
	Display.swap();
	gui.drawList(&DrawList);
	Display.swap();
	Display.fillRec(30, 10, 80, 40, cmdc0de::RGBColor(0, 255, 0));
	Display.swap();
#endif

#if 0
	Display.fillScreen(cmdc0de::RGBColor::BLACK);
	getDisplay()->drawImage(getLogo1());
	Display.swap();
	HAL_Delay(1000);
	Display.fillScreen(cmdc0de::RGBColor::BLACK);
	getDisplay()->drawImage(getLogo2());
	Display.swap();
	HAL_Delay(2000);
#endif
	setCurrentState(getDisplayMenuState());

	//#define TEST_SD_CARD
#ifdef TEST_SD_CARD
	//disk_initialize(0);
	FATFS myFS;
	f_mount(&myFS, "0:", 1);
	DIR d;
	if (FR_OK == f_opendir(&d, "/")) {
		FILINFO fno;
		if (FR_OK == ::f_readdir(&d, &fno)) {
			DBGMSG("%s", &fno.fname[0]);
		}
	}
#endif

#if 0
	flatbuffers::FlatBufferBuilder fbb;
	auto setup = darknet7::CreateSetupAPDirect(fbb, "test", "test", darknet7::WifiMode_WPA2);
	flatbuffers::Offset<darknet7::STMToESPRequest> of = darknet7::CreateSTMToESPRequest(fbb, 1U, darknet7::STMToESPAny_SetupAP, setup.Union());
	darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, of);

	flatbuffers::uoffset_t size = fbb.GetSize();
	uint8_t* p = fbb.GetBufferPointer();

	flatbuffers::Verifier v(p, size);
	if (darknet7::VerifySizePrefixedSTMToESPRequestBuffer(v)) {
		v.GetComputedSize();
		auto t = darknet7::GetSizePrefixedSTMToESPRequest(p);
		auto msg = t->Msg_as_SetupAP();
		if (msg->mode() == darknet7::WifiMode_WPA2) {
			DBGMSG("yeah");
		}
	}
#endif

	//darknet7_led_init();
	DarkNet7::instance->getMcuToMcu().init(&huart1);
	return et;
}

static uint16_t hackMsg = 0;
static uint32_t hackTime = 0;

ErrorType DarkNet7::onRun() {
	MyButtons.processButtons();

	//emit new messages
	if (hackMsg < 2) {
		bool b = false;
		if (hackMsg == 0) {
			b = true;
			++hackMsg;
			hackTime = HAL_GetTick();
		}
		else if ((HAL_GetTick() - hackTime) > 2000) {
			++hackMsg;
			b = true;
		}
		if (b) {
			flatbuffers::FlatBufferBuilder fbb;
			auto r = darknet7::CreateESPRequest(fbb, darknet7::ESPRequestType::ESPRequestType_SYSTEM_INFO);
			auto e = darknet7::CreateSTMToESPRequest(fbb, DarkNet7::instance->nextSeq(), darknet7::STMToESPAny_ESPRequest, r.Union());
			darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, e);
			DarkNet7::instance->getMcuToMcu().send(fbb);
		}
	}
	DarkNet7::instance->getMcuToMcu().process();

	Darknet7BaseState*  rsc = getCurrentState()->run();
	Display.swap();
	//handleLEDS();

	if (rsc.Err.ok()) {
		if (getCurrentState() != rsc.NextMenuToRun) {
			//on state switches reset keyboard and give a 1 second pause on reading from keyboard.
			MyButtons.reset();
			setCurrentState(rsc.NextMenuToRun);
		}
		else {
			if (getCurrentState() != DarkNet7::instance->getGameOfLifeState()
				&& (HAL_GetTick() - MyButtons.lastTickButtonPushed()) > (DarkNet7::instance->getContacts()->getSettings().getScreenSaverTime() * 1000 * 60)) {
				setCurrentState(DarkNet7::instance->getGameOfLifeState());
			}
		}
	}
	else {
		//setCurrentState(StateCollection::getDisplayMessageState(
		//		StateCollection::getDisplayMenuState(), (const char *)"Run State Error....", uint16_t (2000)));
	}

	return ErrorType();
}
