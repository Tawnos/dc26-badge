#include "button_info.h"

ButtonInfo::ButtonInfo() :
	ButtonState(0), LastButtonState(0), LastTickButtonPushed(0) {
	LastTickButtonPushed = HAL_GetTick();
}

void ButtonInfo::processButtons() {
	LastButtonState = ButtonState;
	ButtonState = 0;
	if (HAL_GPIO_ReadPin(MID_BUTTON1_GPIO_Port, MID_BUTTON1_Pin) == GPIO_PIN_RESET) {
		ButtonState |= Mid;
	}
	if (HAL_GPIO_ReadPin(BUTTON_RIGHT_GPIO_Port, BUTTON_RIGHT_Pin) == GPIO_PIN_RESET) {
		ButtonState |= Right;
	}
	if (HAL_GPIO_ReadPin(BUTTON_LEFT_GPIO_Port, BUTTON_LEFT_Pin) == GPIO_PIN_RESET) {
		ButtonState |= Left;
	}
	if (HAL_GPIO_ReadPin(BUTTON_UP_GPIO_Port, BUTTON_UP_Pin) == GPIO_PIN_RESET) {
		ButtonState |= Up;
	}
	if (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_Port, BUTTON_DOWN_Pin) == GPIO_PIN_RESET) {
		ButtonState |= Down;
	}
	if (HAL_GPIO_ReadPin(BUTTON_FIRE1_GPIO_Port, BUTTON_FIRE1_Pin) == GPIO_PIN_RESET) {
		ButtonState |= Fire;
	}
	if (ButtonState != 0) {
		LastTickButtonPushed = HAL_GetTick();
	}
}

uint32_t ButtonInfo::lastTickButtonPushed() {
	return LastTickButtonPushed;
}
