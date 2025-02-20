/*
 * dc26.h
 *
 *  Created on: Jul 10, 2018
 *      Author: cmdc0de
 */

#ifndef MAIN_DC26_H_
#define MAIN_DC26_H_

class MCUToMCUTask;
class BluetoothTask;
class DisplayHandlerTask;

MCUToMCUTask &getMCUToMCU();
BluetoothTask &getBLETask();
DisplayHandlerTask &getDisplayTask();

#endif /* MAIN_DC26_H_ */
