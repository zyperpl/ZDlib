#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "Image.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "Window.hpp"
#include "Screen.hpp"

class Renderer
{
public:
  virtual ~Renderer() {}

  virtual std::shared_ptr<Window> add_window(const WindowParameters &params) = 0;
  virtual void add_screen(std::shared_ptr<Screen> screen)
  {
    screens.push_back(screen);
  }

  virtual void set_window_current(size_t index)
  {
    current_window_index = index;
  }

  virtual std::shared_ptr<Window> window() const
  {
    return windows.at(current_window_index);
  }

  virtual void remove_window(size_t index)
  {
    windows.erase(windows.begin() + index);
  }

  bool is_window_open() { return !windows.empty() && window()->is_open(); }

  virtual void clear() = 0;
  virtual void update() = 0;
  virtual void render() = 0;

protected:
  std::vector<std::shared_ptr<Window>> windows;
  std::vector<std::shared_ptr<Screen>> screens;
  size_t current_window_index { 0 };

  friend class Painter;

private:
};
