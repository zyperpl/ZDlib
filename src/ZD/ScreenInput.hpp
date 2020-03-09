#pragma once

#include "Input.hpp"
#include "Screen.hpp"

class ScreenMouse : public Mouse
{
public:
  ScreenMouse(const Screen &screen)
  : screen { screen }
  {
  }

  virtual const MousePosition<long> position() const;

private:
  const Screen &screen;
};

class ScreenInput : public Input
{
public:
  ScreenInput(const Screen &screen)
  {
    mouse_data = std::make_shared<ScreenMouse>(screen);
  }

private:
};
