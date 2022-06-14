#pragma once

#ifndef BUTTON_INFO_H_
#define BUTTON_INFO_H_

#include <type_traits>
extern uint32_t HAL_GetTick();

enum class ButtonPress : uint8_t
{
   None = 0x00,
   Left = 0x01,
   Right = 0x02,
   Up = 0x04,
   Down = 0x08,
   Mid = 0x10,
   Fire = 0x20
};


inline bool operator == (ButtonPress lhs, std::underlying_type_t<ButtonPress> rhs)
{
   using T = std::underlying_type_t <ButtonPress>;
   return static_cast<T>(lhs) == static_cast<T>(rhs);
}

inline bool operator != (ButtonPress lhs, std::underlying_type_t <ButtonPress> rhs)
{
   return !(lhs == rhs);
}


inline ButtonPress operator ~ (ButtonPress lhs)
{
   using T = std::underlying_type_t <ButtonPress>;
   return static_cast<ButtonPress>(~static_cast<T>(lhs));
}

inline ButtonPress operator | (ButtonPress lhs, ButtonPress rhs)
{
   using T = std::underlying_type_t <ButtonPress>;
   return static_cast<ButtonPress>(static_cast<T>(lhs) | static_cast<T>(rhs));
}


inline ButtonPress& operator |= (ButtonPress& lhs, ButtonPress rhs)
{
   lhs = lhs | rhs;
   return lhs;
}

inline ButtonPress operator & (ButtonPress lhs, ButtonPress rhs)
{
   using T = std::underlying_type_t <ButtonPress>;
   return static_cast<ButtonPress>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

inline ButtonPress operator &= (ButtonPress& lhs, ButtonPress rhs)
{
   lhs = lhs & rhs;
   return lhs;
}

class ButtonInfo
{
public:
   ButtonInfo() :LastTickButtonPushed(HAL_GetTick()) {}

   void onButtonDown(ButtonPress b) {
      LastTickButtonPushed = HAL_GetTick();
      buttonState |= b;
   }
   void onButtonUp(ButtonPress b) {
      buttonState = buttonState & ~b;
   }

   void reset()
   {
      buttonState = LastButtonState = ButtonPress::None;
   }

   bool areTheseButtonsDown(const ButtonPress& b)
   {
      return (buttonState & b) == b;
   }

   bool isAnyOfTheseButtonDown(const ButtonPress& b)
   {
      return (buttonState & b) != 0;
   }

   bool isAnyButtonDown()
   {
      return buttonState != 0;
   }

   bool wereTheseButtonsReleased(const ButtonPress& b)
   {
      //last state must match these buttons and current state must have none of these buttons
      return (LastButtonState & b) == b && (buttonState & b) == 0;
   }

   bool wereAnyOfTheseButtonsReleased(const ButtonPress& b)
   {
      //last state must have at least 1 of the buttons and at least 1 of the buttons must not be down now
      return (LastButtonState & b) != 0 && !isAnyOfTheseButtonDown(b);
   }

   bool wasAnyButtonReleased()
   {
      return buttonState != LastButtonState && LastButtonState != 0;
   }

   uint32_t& lastTickButtonPushed() { return LastTickButtonPushed; }

   void processButtons();
private:
   ButtonPress buttonState{ ButtonPress::None };
   ButtonPress LastButtonState{ ButtonPress::None };
   uint32_t LastTickButtonPushed{ 0 };
};

#endif