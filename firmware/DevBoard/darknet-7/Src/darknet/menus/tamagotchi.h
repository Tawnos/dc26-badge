/*
 * tamagotchi.h
 *
 *  Created on: May 30, 2018
 *      Author: dcomes
 */

#ifndef DARKNET_MENUS_TAMAGOTCHI_H_
#define DARKNET_MENUS_TAMAGOTCHI_H_


#include "darknet7_base_state.h"


class Tamagotchi: public Darknet7BaseState {
public:
	Tamagotchi(DarkNet7* darknet);
	virtual ~Tamagotchi();
protected:
	virtual cmdc0de::ErrorType onInit();
	virtual Darknet7BaseState*  onRun();
	virtual cmdc0de::ErrorType onShutdown();
private:

};




#endif /* DARKNET_MENUS_TAMAGOTCHI_H_ */
