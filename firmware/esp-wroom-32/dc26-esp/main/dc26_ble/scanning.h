#ifndef DC26_BLUETOOTH_SCANNING
#define DC26_BLUETOOTH_SCANNING

#include "esp_system.h"
#include "esp_log.h"
#include "ble.h"
#include <map>
#include "../lib/Task.h"
#include "../lib/ble/BLEDevice.h"

class MyScanCallbacks : public BLEAdvertisedDeviceCallbacks {
public:
	unsigned int detected;
	uint16_t exposures = 0;
	void reset(void);
	uint16_t getExposures(void);
	uint16_t getCures(void);
	std::map<std::string,std::string> getResults(void);
	void setFilter(uint8_t val);
	void onResult(BLEAdvertisedDevice advertisedDevice);
private:
	uint8_t filter;
	int min_RSSI;
	uint16_t cures = 0;
	std::map<std::string,int> RSSIs;
	std::map<std::string,std::string> results;
	unsigned int getNumberOfResults();
	void filterBadges(BLEAdvertisedDevice device);
	void filterNPCs(BLEAdvertisedDevice device);
protected:
};

#endif // DC26_BLUETOOTH_SCANNING
