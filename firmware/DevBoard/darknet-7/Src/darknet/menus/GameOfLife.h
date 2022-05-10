#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include "darknet7_base_state.h"
#include "../libstm32/utility/bitarray.h"

class GameOfLife: public Darknet7BaseState {
public:
	using Darknet7BaseState::Darknet7BaseState;
	virtual ~GameOfLife() = default;

public:
	static const int width = 128;
	static const int height = 128;
	static const int num_slots = width*height;
	static const int sizeof_buffer = (num_slots/8)+1;
	static uint8_t Buffer[sizeof_buffer];
	static const uint32_t NEED_DRAW = 1<< Darknet7BaseState::SHIFT_FROM_BASE;
protected:
	virtual cmdc0de::ErrorType onInit() override;
	virtual Darknet7BaseState*  onRun() override;
	virtual cmdc0de::ErrorType onShutdown() override;
	void initGame();
	bool life(cmdc0de::BitArray &array, char choice, short width, short height, cmdc0de::BitArray &temp);
	enum INTERNAL_STATE {
		INIT, MESSAGE, TIME_WAIT, GAME, SLEEP
	};
	bool shouldDisplayMessage();
private:
	uint16_t Generations{0};
	uint16_t CurrentGeneration{ 0 };
	uint8_t Neighborhood{0};
	//uint64_t gol[height];
	cmdc0de::BitArray GOL{ &Buffer[0], num_slots, 1 };
	char UtilityBuf[64]{0};
	INTERNAL_STATE InternalState{ GameOfLife::GAME };
	uint32_t DisplayMessageUntil{0};
};

#endif
