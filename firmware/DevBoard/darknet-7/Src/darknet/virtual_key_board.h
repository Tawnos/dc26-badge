/*
 * virtual_key_board.h
 *
 *  Created on: Jul 14, 2018
 *      Author: dcomes
 */
#pragma once
#ifndef DARKNET_MENUS_VIRTUAL_KEY_BOARD_H_
#define DARKNET_MENUS_VIRTUAL_KEY_BOARD_H_

#include "gui.h"
#include "button_info.h"
#include <libstm32/display/display_device.h>
#include <libstm32/observer/event_bus.h>
#include <libstm32/rgbcolor.h>

class DarkNet7;
class VirtualKeyBoard
{
public:
   class InputHandleContext
   {
   public:
      InputHandleContext(char* b, uint8_t s) : Buf(b), Size(s), CurrentPos(0) {}
      void set(char* b, uint8_t s) { Buf = b; Size = s; CurrentPos = 0; }
      void addChar(char b);
      void backspace();
   private:
      char* Buf;
      uint8_t Size;
      uint8_t CurrentPos;
   };
public:
   typedef cmdc0de::EventBus<1, 2, 2, 3> VKB_EVENT_BUS_TYPE;
   static const char* STDKBLowerCase;
   static const char* STDKBNames;
   static const char* STDCAPS;
public:
   VirtualKeyBoard(cmdc0de::GUI* gui, ButtonInfo* buttons) :gui(gui), buttons(buttons) {}

   void init(const char* vkb, InputHandleContext* ic, int16_t xdisplayPos, int16_t xEndDisplay, int16_t yDisplayPos, const cmdc0de::RGBColor& fontColor,
      const cmdc0de::RGBColor& backgroundColor, const cmdc0de::RGBColor& cursorColor, const char cursorChar = '_');
   void process();



   auto getCursorX()
   {
      return CursorPos % CharsPerRow;
   }


   auto getCursorY()
   {
      return CursorPos / CharsPerRow;

   }

   auto getVKBIndex()
   {
      return CursorPos;
   }


   auto getSelectedChar()
   {
      return VKB[CursorPos];
   }
private:
   const char* VKB{ nullptr };
   uint16_t SizeOfKeyboard{ 0 };
   int16_t XDisplayPos{ 0 };
   int16_t XEndDisplayPos{ cmdc0de::DISPLAY_WIDTH };
   int16_t YDisplayPos{ 0 };
   cmdc0de::RGBColor FontColor{ cmdc0de::RGBColor::WHITE };
   cmdc0de::RGBColor BackGround{ cmdc0de::RGBColor::BLACK };
   cmdc0de::RGBColor CursorColor{ cmdc0de::RGBColor::BLUE };
   char CursorChar{ '_' };
   int16_t CursorPos{ 0 };
   int16_t CharsPerRow{ 0 };
   InputHandleContext* InputContext{ nullptr };
   cmdc0de::GUI* gui{ nullptr };
   ButtonInfo* buttons{ nullptr };
   cmdc0de::DisplayDevice* displayDevice{ nullptr };
};



#endif /* DARKNET_MENUS_VIRTUAL_KEY_BOARD_H_ */
