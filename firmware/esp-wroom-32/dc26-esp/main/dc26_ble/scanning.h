#ifndef DC26_BLUETOOTH_SCANNING
#define DC26_BLUETOOTH_SCANNING

#include "esp_system.h"
#include "esp_log.h"
#include "ble.h"
#include "../lib/Task.h"
#include "../lib/ble/BLEDevice.h"

class MyScanCallbacks : public BLEAdvertisedDeviceCallbacks {
public:
	void reset(void);
	uint16_t getInfections(void);
	uint16_t getCures(void);
	void getResults(void); // TODO
	void setFilter(uint8_t val);
	void onResult(BLEAdvertisedDevice advertisedDevice);
private:
	uint8_t filter;
	uint16_t infections;
	uint16_t cures;
	void * results; // TODO: type should be map
	void filterBadges(BLEAdvertisedDevice device);
	void filterNPCs(BLEAdvertisedDevice device);
protected:
};

#endif // DC26_BLUETOOTH_SCANNING
