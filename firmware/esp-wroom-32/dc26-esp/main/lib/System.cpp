/*
 * System.cpp
 *
 *  Created on: May 27, 2017
 *      Author: kolban
 */

#include "System.h"
#include <esp_system.h>

extern "C" {
#include <esp_heap_caps.h>
}

System::System() {
	// TODO Auto-generated constructor stub

}

System::~System() {
	// TODO Auto-generated destructor stub
}

/**
 * @brief Get the information about the device.
 * @param [out] info The structure to be populated on return.
 * @return N/A.
 */
void System::getChipInfo(esp_chip_info_t *info) {
	::esp_chip_info(info);
} // getChipInfo


/**
 * @brief Retrieve the system wide free heap size.
 * @return The system wide free heap size.
 */
size_t System::getFreeHeapSize() {
	return heap_caps_get_free_size(MALLOC_CAP_8BIT);
} // getFreeHeapSize


/**
 * @brief Retrieve the version of the ESP-IDF.
 * When an application is compiled, it is compiled against a version of the ESP-IDF.
 * This function returns that version.
 */
const char *System::getIDFVersion() {
	return ::esp_get_idf_version();
} // getIDFVersion


/**
 * @brief Get the smallest heap size seen.
 * @return The smallest heap size seen.
 */
size_t System::getMinimumFreeHeapSize() {
	return heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT);
} // getMinimumFreeHeapSize

/**
 * @brief Restart the ESP32.
 */
void System::restart() {
	esp_restart();
}

void System::logSystemInfo() {
	 esp_chip_info_t chipInfo;
	 printf("Free HeapSize: %u\n",System::getFreeHeapSize());
	 printf("Free Min HeapSize: %u\n",System::getMinimumFreeHeapSize());
	 System::getChipInfo(&chipInfo);
	 printf("Model = %d\n", chipInfo.model);
	 printf("Features = %d\n", chipInfo.features);
	 printf("	EMB_FLASH %d\n", (chipInfo.features&CHIP_FEATURE_EMB_FLASH)!=0);
	 printf("	WIFI_BGN %d\n", (chipInfo.features&CHIP_FEATURE_WIFI_BGN)!=0);
	 printf("	BLE %d\n", (chipInfo.features&CHIP_FEATURE_BLE)!=0);
	 printf("	BT %d\n", (chipInfo.features&CHIP_FEATURE_BT)!=0);
	 printf("Cores = %d\n", (int)chipInfo.cores);
	 printf("revision = %d\n", (int)chipInfo.revision);
	 printf("IDF Version = %s\n", ::esp_get_idf_version());
}

