#ifndef BUTTON_INFO_H_
#define BUTTON_INFO_H_
#include <type_traits>

enum class Button : uint8_t {
  None = 0x00,
  Left = 0x01,
  Right = 0x02,
  Up = 0x04,
  Down = 0x08,
  Mid = 0x10,
  Fire = 0x20
};


inline bool operator == (Button lhs, std::underlying_type_t<Button> rhs)
{
  using T = std::underlying_type_t <Button>;
  return static_cast<T>(lhs) == static_cast<T>(rhs);
}

inline bool operator != (Button lhs, std::underlying_type_t <Button> rhs)
{
  return !(lhs == rhs);
}

inline Button operator | (Button lhs, Button rhs)
{
  using T = std::underlying_type_t <Button>;
  return static_cast<Button>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline Button operator & (Button lhs, Button rhs)
{
  using T = std::underlying_type_t <Button>;
  return static_cast<Button>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

inline Button& operator |= (Button& lhs, Button rhs)
{
  lhs = lhs | rhs;
  return lhs;
}

class ButtonInfo {
public:
   ButtonInfo() :LastTickButtonPushed(HAL_GetTick()) {}

  void reset() {
    ButtonState = LastButtonState = Button::None;
  }

  bool areTheseButtonsDown(const Button& b) {
    return (ButtonState & b) == b;
  }

  bool isAnyOfTheseButtonDown(const Button& b) {
    return (ButtonState & b) != 0;
  }

  bool isAnyButtonDown() {
    return ButtonState != 0;
  }

  bool wereTheseButtonsReleased(const Button& b) {
    //last state must match these buttons and current state must have none of these buttons
    return (LastButtonState & b) == b && (ButtonState & b) == 0;
  }

  bool wereAnyOfTheseButtonsReleased(const Button& b) {
    //last state must have at least 1 of the buttons and at least 1 of the buttons must not be down now
    return (LastButtonState & b) != 0 && !isAnyOfTheseButtonDown(b);
  }

  bool wasAnyButtonReleased() {
    return ButtonState != LastButtonState && LastButtonState != 0;
  }

  uint32_t lastTickButtonPushed() { return LastTickButtonPushed; }

protected:
  void processButtons();
  friend class DarkNet7;
private:
  Button ButtonState;
  Button LastButtonState;
  uint32_t LastTickButtonPushed{ 0 };
};

#endif