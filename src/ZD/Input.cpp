#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <unordered_map>

#include "Input.hpp"

static std::unordered_map<int, Key> KEYS_GLFW {
  { GLFW_KEY_UNKNOWN, Key::Invalid },
  { GLFW_KEY_SPACE, Key::Space },
  { GLFW_KEY_APOSTROPHE, Key::Apostrophe },
  { GLFW_KEY_COMMA, Key::Comma },
  { GLFW_KEY_MINUS, Key::Minus },
  { GLFW_KEY_PERIOD, Key::Period },
  { GLFW_KEY_SLASH, Key::Slash },
  { GLFW_KEY_0, Key::Num0 },
  { GLFW_KEY_1, Key::Num1 },
  { GLFW_KEY_2, Key::Num2 },
  { GLFW_KEY_3, Key::Num3 },
  { GLFW_KEY_4, Key::Num4 },
  { GLFW_KEY_5, Key::Num5 },
  { GLFW_KEY_6, Key::Num6 },
  { GLFW_KEY_7, Key::Num7 },
  { GLFW_KEY_8, Key::Num8 },
  { GLFW_KEY_9, Key::Num9 },
  { GLFW_KEY_SEMICOLON, Key::Semicolon },
  { GLFW_KEY_EQUAL, Key::Equal },
  { GLFW_KEY_A, Key::A },
  { GLFW_KEY_B, Key::B },
  { GLFW_KEY_C, Key::C },
  { GLFW_KEY_D, Key::D },
  { GLFW_KEY_E, Key::E },
  { GLFW_KEY_F, Key::F },
  { GLFW_KEY_G, Key::G },
  { GLFW_KEY_H, Key::H },
  { GLFW_KEY_I, Key::I },
  { GLFW_KEY_J, Key::J },
  { GLFW_KEY_K, Key::K },
  { GLFW_KEY_L, Key::L },
  { GLFW_KEY_M, Key::M },
  { GLFW_KEY_N, Key::N },
  { GLFW_KEY_O, Key::O },
  { GLFW_KEY_P, Key::P },
  { GLFW_KEY_Q, Key::Q },
  { GLFW_KEY_R, Key::R },
  { GLFW_KEY_S, Key::S },
  { GLFW_KEY_T, Key::T },
  { GLFW_KEY_U, Key::U },
  { GLFW_KEY_V, Key::V },
  { GLFW_KEY_W, Key::W },
  { GLFW_KEY_X, Key::X },
  { GLFW_KEY_Y, Key::Y },
  { GLFW_KEY_Z, Key::Z },
  { GLFW_KEY_LEFT_BRACKET, Key::LeftBracket },
  { GLFW_KEY_BACKSLASH, Key::Backslash },
  { GLFW_KEY_RIGHT_BRACKET, Key::RightBracket },
  { GLFW_KEY_GRAVE_ACCENT, Key::Accent },
  { GLFW_KEY_WORLD_1, Key::Invalid },
  { GLFW_KEY_WORLD_2, Key::Invalid },
  { GLFW_KEY_ESCAPE, Key::Escape },
  { GLFW_KEY_ENTER, Key::Return },
  { GLFW_KEY_TAB, Key::Tab },
  { GLFW_KEY_BACKSPACE, Key::Backspace },
  { GLFW_KEY_INSERT, Key::Insert },
  { GLFW_KEY_DELETE, Key::Delete },
  { GLFW_KEY_RIGHT, Key::Right },
  { GLFW_KEY_LEFT, Key::Left },
  { GLFW_KEY_DOWN, Key::Down },
  { GLFW_KEY_UP, Key::Up },
  { GLFW_KEY_PAGE_UP, Key::PageUp },
  { GLFW_KEY_PAGE_DOWN, Key::PageDown },
  { GLFW_KEY_HOME, Key::Home },
  { GLFW_KEY_END, Key::End },
  { GLFW_KEY_CAPS_LOCK, Key::CapsLock },
  { GLFW_KEY_SCROLL_LOCK, Key::ScrollLock },
  { GLFW_KEY_NUM_LOCK, Key::NumLock },
  { GLFW_KEY_PRINT_SCREEN, Key::PrintScreen },
  { GLFW_KEY_PAUSE, Key::Pause },
  { GLFW_KEY_F1, Key::F1 },
  { GLFW_KEY_F2, Key::F2 },
  { GLFW_KEY_F3, Key::F3 },
  { GLFW_KEY_F4, Key::F4 },
  { GLFW_KEY_F5, Key::F5 },
  { GLFW_KEY_F6, Key::F6 },
  { GLFW_KEY_F7, Key::F7 },
  { GLFW_KEY_F8, Key::F8 },
  { GLFW_KEY_F9, Key::F9 },
  { GLFW_KEY_F10, Key::F10 },
  { GLFW_KEY_F11, Key::F11 },
  { GLFW_KEY_F12, Key::F12 },
  { GLFW_KEY_F13, Key::F13 },
  { GLFW_KEY_F14, Key::F14 },
  { GLFW_KEY_F15, Key::F15 },
  { GLFW_KEY_F16, Key::F16 },
  { GLFW_KEY_F17, Key::F17 },
  { GLFW_KEY_F18, Key::F18 },
  { GLFW_KEY_F19, Key::F19 },
  { GLFW_KEY_F20, Key::F20 },
  { GLFW_KEY_F21, Key::F21 },
  { GLFW_KEY_F22, Key::F22 },
  { GLFW_KEY_F23, Key::F23 },
  { GLFW_KEY_F24, Key::F24 },
  { GLFW_KEY_F25, Key::F25 },
  { GLFW_KEY_KP_0, Key::Keypad0 },
  { GLFW_KEY_KP_1, Key::Keypad1 },
  { GLFW_KEY_KP_2, Key::Keypad2 },
  { GLFW_KEY_KP_3, Key::Keypad3 },
  { GLFW_KEY_KP_4, Key::Keypad4 },
  { GLFW_KEY_KP_5, Key::Keypad5 },
  { GLFW_KEY_KP_6, Key::Keypad6 },
  { GLFW_KEY_KP_7, Key::Keypad7 },
  { GLFW_KEY_KP_8, Key::Keypad8 },
  { GLFW_KEY_KP_9, Key::Keypad9 },
  { GLFW_KEY_KP_DECIMAL, Key::KeypadDecimal },
  { GLFW_KEY_KP_DIVIDE, Key::KeypadDivide },
  { GLFW_KEY_KP_MULTIPLY, Key::KeypadMultiply },
  { GLFW_KEY_KP_SUBTRACT, Key::KeypadSubtract },
  { GLFW_KEY_KP_ADD, Key::KeypadAdd },
  { GLFW_KEY_KP_ENTER, Key::KeypadReturn },
  { GLFW_KEY_KP_EQUAL, Key::KeypadEqual },
  { GLFW_KEY_LEFT_SHIFT, Key::LeftShift },
  { GLFW_KEY_LEFT_CONTROL, Key::LeftControl },
  { GLFW_KEY_LEFT_ALT, Key::LeftAlt },
  { GLFW_KEY_LEFT_SUPER, Key::LeftSuper },
  { GLFW_KEY_RIGHT_SHIFT, Key::RightShift },
  { GLFW_KEY_RIGHT_CONTROL, Key::RightControl },
  { GLFW_KEY_RIGHT_ALT, Key::RightAlt },
  { GLFW_KEY_RIGHT_SUPER, Key::RightSuper },
  { GLFW_KEY_MENU, Key::Menu },
  { GLFW_KEY_LAST, Key::Menu }
};

static std::unordered_map<int, MouseButton> MOUSE_BUTTONS_GLFW {
  { GLFW_MOUSE_BUTTON_1, MouseButton::Left },
  { GLFW_MOUSE_BUTTON_2, MouseButton::Right },
  { GLFW_MOUSE_BUTTON_3, MouseButton::Middle },
  { GLFW_MOUSE_BUTTON_4, MouseButton::Invalid },
  { GLFW_MOUSE_BUTTON_5, MouseButton::Invalid },
  { GLFW_MOUSE_BUTTON_6, MouseButton::Invalid },
  { GLFW_MOUSE_BUTTON_7, MouseButton::Invalid },
  { GLFW_MOUSE_BUTTON_8, MouseButton::Invalid },
  { GLFW_MOUSE_BUTTON_LAST, MouseButton::Invalid },
  { GLFW_MOUSE_BUTTON_LEFT, MouseButton::Left },
  { GLFW_MOUSE_BUTTON_RIGHT, MouseButton::Right },
  { GLFW_MOUSE_BUTTON_MIDDLE, MouseButton::Middle }
};

Input_GLFW::Input_GLFW()
{
  for (const auto& [id, key] : KEYS_GLFW)
  {
    keys[key] = 0;
  }
  for (const auto& [id, button] : MOUSE_BUTTONS_GLFW)
  {
    mouse_data.buttons[button] = 0;
  }
}

void Input_GLFW::update_key(const int k, const int value)
{
  const Key key = KEYS_GLFW.at(k);
  keys[key] = value;
}

void Input_GLFW::update_mouse_button(const int mb, const int value)
{
  const MouseButton button = MOUSE_BUTTONS_GLFW.at(mb);
  mouse_data.buttons[button] = value;
}

void Input_GLFW::update_mouse_position(
  const double mx, const double my, const Size& window_size,
  const Size& canvas_size)
{
  mouse_data.position_window_space.x = mx;
  mouse_data.position_window_space.y = my;

  const double scl_x =
    (double)(window_size.width()) / (double)(canvas_size.width());
  const double scl_y =
    (double)(window_size.height()) / (double)(canvas_size.height());

  double scl = scl_x;
  if (scl > scl_y)
    scl = scl_y;

  const int x_offset =
    (window_size.width() - (scl * canvas_size.width())) / 2.0f;
  const int y_offset =
    (window_size.height() - (scl * canvas_size.height())) / 2.0f;

  mouse_data.position_screen_space.x = ((mx - x_offset) / scl) + 0.5;
  mouse_data.position_screen_space.y = ((my - y_offset) / scl) + 0.5;
}

void Input_GLFW::add_mouse_scroll(const double scroll_x, const double scroll_y)
{
  mouse_data.scroll_x_offset += scroll_x;
  mouse_data.scroll_y_offset += scroll_y;
}
