#ifndef DC26_BLUETOOTH
#define DC26_BLUETOOTH

#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "../lib/Task.h"
#include "../lib/ble/BLEDevice.h"
#include "../mcu_to_mcu.h"
#include "pairing.h"
#include "scanning.h"
#include "security.h"
#include <nvs_flash.h>
	

class BluetoothTask : public Task {
public:
	static const char *LOGTAG;

	BLEDevice *pDevice;
	BLEServer *pServer;
	BLEClient *pClient;
	BLEService *pService;
	BLEScan *pScan;
	BLEAddress *connectedDevice;
	BLESecurity *pSecurity;
	MySecurity *pMySecurity;
	BLEAdvertising *pAdvertising;
	MyScanCallbacks *pScanCallbacks;
	BLECharacteristic *pUartCisoCharacteristic;
	BLECharacteristic *pUartCosiCharacteristic;
	BLEAdvertisementData adv_data;

	// nvs handle
	nvs_handle nvs_fp;
	bool nvs_file_opened = false;

	// Advertisement data
	bool advertising_enabled = false;
	std::string adv_name = "DN1";
	char man_buf[7]= {'D','N','A','\0','\0','\0','\0'};

	bool isActingClient = false;
	bool isActingServer = false;

	// STM to ESP Queue
	static const int STM_MSG_QUEUE_SIZE = 3;
	static const int STM_MSG_ITEM_SIZE = sizeof(MCUMessage *);
	StaticQueue_t STMQueue;
	QueueHandle_t STMQueueHandle = nullptr;
	uint8_t fromSTMBuffer[STM_MSG_QUEUE_SIZE*STM_MSG_ITEM_SIZE];
	QueueHandle_t getQueueHandle() {return STMQueueHandle;}

public: // API
	void getInfectionData(void);
	void toggleAdvertising(const darknet7::STMToESPRequest* m);
	void setDeviceType(uint8_t devtype);
	void setDeviceName(const darknet7::STMToESPRequest* m);
	void setExposureData(const darknet7::STMToESPRequest* m);
	void setInfectionData(const darknet7::STMToESPRequest* m);
	void setCureData(const darknet7::STMToESPRequest* m);
	void scanForDevices(const darknet7::STMToESPRequest* m);
	void pairWithDevice(const darknet7::STMToESPRequest* m);
	void sendPINConfirmation(const darknet7::STMToESPRequest* m);
	void sendDataToDevice(const darknet7::STMToESPRequest* m);
	void sendDNPairComplete(const darknet7::STMToESPRequest* m);
	void disconnect();

public:
	BluetoothTask(const std::string &tName, uint16_t stackSize=10000, uint8_t p=5);
	bool init();
	void commandHandler(MCUMessage* cmd);
	virtual void run(void *data);
	virtual ~BluetoothTask();
protected:
	void startAdvertising(void);
	void stopAdvertising(void);
	void refreshAdvertisementData(void);
};


#endif // DC26_BLUETOOTH
