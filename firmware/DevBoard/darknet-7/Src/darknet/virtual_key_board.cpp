/*
 * virtual_key_board.cpp
 *
 *  Created on: Jul 14, 2018
 *      Author: cmdc0de
 */

#include "virtual_key_board.h"
#include "darknet7.h"
#include <string>

using cmdc0de::RGBColor;

const char* VirtualKeyBoard::STDKBLowerCase = "abcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()-";
const char* VirtualKeyBoard::STDKBNames = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()-_";
const char* VirtualKeyBoard::STDCAPS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

VirtualKeyBoard::VirtualKeyBoard(DarkNet7* darknet)
   : displayDevice(darknet->getDisplay()), buttons(darknet->getButtonInfo())
{}

void VirtualKeyBoard::InputHandleContext::addChar(char b)
{
   if (Buf != 0)
   {
      if (CurrentPos < (Size - 1))
      {
         Buf[CurrentPos] = b;
         ++CurrentPos;
      }
      else
      {
         //do nothing
      }
   }
}

//Remember CurrentPos here means the current position in the input buffer not the position in the keyboard like below.
void VirtualKeyBoard::InputHandleContext::backspace()
{
   Buf[CurrentPos] = '\0';
   --CurrentPos;
}

////////
void VirtualKeyBoard::init(const char* vkb, 
   InputHandleContext* ic, 
   int16_t xdisplayPos, int16_t xEndDisplay, 
   int16_t yDisplayPos, 
   const cmdc0de::RGBColor& fontColor,
   const cmdc0de::RGBColor& backgroundColor,
   const cmdc0de::RGBColor& cursorColor,
   char cursorChar)
{
   VKB = vkb;
   SizeOfKeyboard = strlen(VKB);
   XDisplayPos = xdisplayPos;
   XEndDisplayPos = xEndDisplay;
   YDisplayPos = yDisplayPos;
   FontColor = fontColor;
   BackGround = backgroundColor;
   CursorColor = cursorColor;
   CursorChar = cursorChar;
   CursorPos = 0;
   uint8_t FontPixelWidth = displayDevice->getFont()->FontWidth;
   CharsPerRow = (XEndDisplayPos - XDisplayPos) / FontPixelWidth;
   InputContext = ic;
}



void VirtualKeyBoard::process()
{
   if (buttons->wereAnyOfTheseButtonsReleased(ButtonPress::Left))
   {
      if (CursorPos > 0)	--CursorPos;
   }
   else if (buttons->wereAnyOfTheseButtonsReleased(ButtonPress::Right))
   {
      if (CursorPos < SizeOfKeyboard) CursorPos++;
      else CursorPos = 0;
   }
   else if (buttons->wereAnyOfTheseButtonsReleased(ButtonPress::Up))
   {
      if (CursorPos >= CharsPerRow) CursorPos -= CharsPerRow;
   }
   else if (buttons->wereAnyOfTheseButtonsReleased(ButtonPress::Down))
   {
      CursorPos += CharsPerRow;
      if (CursorPos > SizeOfKeyboard) CursorPos = SizeOfKeyboard - 1;
   }
   else if (buttons->wereAnyOfTheseButtonsReleased(ButtonPress::Fire))
   {
      if (InputContext)
      {
         InputContext->addChar(getSelectedChar());
      }
   }
   uint16_t y = 0;
   const char* ptr = VKB;
   uint8_t FontPixelHeight = displayDevice->getFont()->FontHeight;
   uint8_t FontPixelWidth = displayDevice->getFont()->FontWidth;
   uint8_t cursorRow = getCursorY();
   uint8_t curosrColumn = getCursorX();
   for (int i = 0; i < SizeOfKeyboard && y < (cmdc0de::DISPLAY_HEIGHT - (y * FontPixelHeight)); i += CharsPerRow, ++y)
   {
      displayDevice->drawString(XDisplayPos, (YDisplayPos + (y * FontPixelHeight)), ptr, FontColor, BackGround, 1, false, CharsPerRow);
      if (y == cursorRow)
      {
         if((HAL_GetTick()%1000)<500) {
            displayDevice->drawString(XDisplayPos+(curosrColumn*FontPixelWidth), YDisplayPos+(y*FontPixelHeight), "_", CursorColor, BackGround, 1, false);
         }
      }
      ptr = ptr + CharsPerRow;
   }
}
