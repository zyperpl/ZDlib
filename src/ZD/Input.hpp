#pragma once

#include <memory>
#include <unordered_map>

#include "3rd/glm/glm.hpp"
#include "Size.hpp"

enum class Key
{
  Backspace,
  Tab,
  Return,
  Escape,
  Space,
  Apostrophe,
  Comma,
  Minus,
  Period,
  Slash,
  Num0,
  Num1,
  Num2,
  Num3,
  Num4,
  Num5,
  Num6,
  Num7,
  Num8,
  Num9,
  Semicolon,
  Equal,
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  LeftBracket,
  Backslash,
  RightBracket,
  Accent,
  Insert,
  Delete,
  Right,
  Left,
  Down,
  Up,
  PageUp,
  PageDown,
  Home,
  End,
  CapsLock,
  ScrollLock,
  NumLock,
  PrintScreen,
  Pause,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,
  F13,
  F14,
  F15,
  F16,
  F17,
  F18,
  F19,
  F20,
  F21,
  F22,
  F23,
  F24,
  F25,
  Keypad0,
  Keypad1,
  Keypad2,
  Keypad3,
  Keypad4,
  Keypad5,
  Keypad6,
  Keypad7,
  Keypad8,
  Keypad9,
  KeypadDecimal,
  KeypadDivide,
  KeypadMultiply,
  KeypadSubtract,
  KeypadAdd,
  KeypadReturn,
  KeypadEqual,
  LeftShift,
  LeftControl,
  LeftAlt,
  LeftSuper,
  RightShift,
  RightControl,
  RightAlt,
  RightSuper,
  Menu,
  Invalid
};

enum class MouseButton
{
  Left,
  Middle,
  Right,
  Invalid
};

template<typename T>
struct MousePosition
{
  T x;
  T y;

  operator glm::vec<2, T>() const { return { x, y }; }
  operator glm::vec2() const { return { x, y }; }
};

class Mouse
{
public:
  inline const MousePosition<double> position_window() const
  {
    return position_window_space;
  }
  virtual inline const MousePosition<long> position() const
  {
    return position_view_space;
  }
  inline double peek_scroll_x() const { return scroll_x_offset; }
  inline double peek_scroll_y() const { return scroll_y_offset; }
  inline double scroll_x() const
  {
    double sx = scroll_x_offset;
    scroll_x_offset = 0;
    return sx;
  }
  inline double scroll_y() const
  {
    double sy = scroll_y_offset;
    scroll_y_offset = 0;
    return sy;
  }

  inline int button(MouseButton button) const { return buttons.at(button); }

protected:
  MousePosition<double> position_window_space { 0.0, 0.0 };
  MousePosition<long> position_view_space { -1, -1 };
  mutable double scroll_y_offset { 0.0 };
  mutable double scroll_x_offset { 0.0 };
  std::unordered_map<MouseButton, int> buttons;

  friend class Input;
  friend class Input_GLFW;
};

class Input
{
public:
  Input() { mouse_data = std::make_shared<Mouse>(); }
  inline int key(const Key &k) const { return keys.at(k); }
  inline int consume_key(const Key &k) const
  {
    auto v = keys.at(k);
    keys.at(k) = 0;
    return v;
  }
  inline const Mouse &mouse() const { return *mouse_data.get(); }

protected:
  mutable std::unordered_map<Key, int> keys;
  std::shared_ptr<Mouse> mouse_data;
  friend class Window;
  friend class Input_GLFW;
};

class Input_GLFW
{
public:
  Input_GLFW();
  Input_GLFW(std::shared_ptr<Input> input);
  const Input *get() { return input.get(); }
  void update_key(const int k, const int value);
  void update_mouse_button(const int mb, const int value);
  void update_mouse_position(
    const double mx, const double my, const Size &window_size,
    const Size &canvas_size);
  void add_mouse_scroll(const double scroll_x, const double scroll_y);

private:
  void reset_data();
  std::shared_ptr<Input> input;
};
