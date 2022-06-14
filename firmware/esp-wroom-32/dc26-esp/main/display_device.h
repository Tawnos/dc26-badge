#pragma once

#include <stdint.h>
#include <fonts/fonts.h>

class DisplayDevice
{
public:
	virtual void Fill(uint8_t color) = 0;
	virtual void GotoXY(uint8_t x, uint8_t y) = 0;
	virtual void Puts(const char* s, const FontDef_t* font, uint8_t color) = 0;
	virtual void UpdateScreen() = 0;
};