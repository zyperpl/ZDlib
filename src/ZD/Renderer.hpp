#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "Image.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "Window.hpp"

class Painter;

class Renderer
{
  public:
    virtual ~Renderer() {}

    virtual const Window *add_window(const WindowParameters &params) = 0;

    virtual void set_window_current(size_t index) { 
      current_window_index = index; 
    }

    virtual Window &get_window() const { 
      return *windows.at(current_window_index).get(); 
    }

    virtual void remove_window(size_t index) {
      windows.erase(windows.begin() + index);
    }

    bool is_window_open() {
      return window()->is_open();
    }

    virtual std::shared_ptr<Image> get_main_screen_image() {
      if (!main_screen_image) 
      {
        initialize_main_screen_image();
      }
      return main_screen_image;
    }

    virtual void clear() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
  protected:
    std::vector<std::unique_ptr<Window>> windows;
    size_t current_window_index{0};

    inline Window* window() {
      return windows.at(current_window_index).get();
    }

    virtual void initialize_main_screen_image() {
      int w = window()->get_width();
      int h = window()->get_height();
      auto format = window()->get_format();
      main_screen_image = Image::create(Size(w, h), format);
    }

    std::shared_ptr<Image> main_screen_image;

    friend class Painter;
  private:

};
