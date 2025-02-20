/*
 * System.h
 *
 *  Created on: May 27, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_SYSTEM_H_
#define COMPONENTS_CPP_UTILS_SYSTEM_H_
#include <stdint.h>
#include <esp_system.h>

/**
 * @brief System wide functions.
 */
class System {
public:
	System();
	virtual ~System();
	static void logSystemInfo();
	static void getChipInfo(esp_chip_info_t *info);
	static size_t getFreeHeapSize();
	static const char *getIDFVersion();
	static size_t getMinimumFreeHeapSize();
	static void restart();
};

#endif /* COMPONENTS_CPP_UTILS_SYSTEM_H_ */
