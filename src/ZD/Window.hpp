#pragma once

#include <string_view>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Color.hpp"
#include "Size.hpp"

struct WindowParameters
{
  int width{0};
  int height{0};
  std::string_view name{""};

  WindowParameters(Size size, std::string_view name)
    : width{size.width()}, height{size.height()}, name{name}
  { }

  WindowParameters(std::string_view name)
    : name{name}
  { }

};

class Window
{
public:
  virtual ~Window() {};

  virtual void init() = 0;
  virtual void show() = 0;
  virtual void hide() = 0;
  virtual void kill() = 0;
  virtual void set_current() {};
  virtual bool is_open() const { return true; } 

  int get_width() const { return width; }
  int get_height() const { return height; }
  int get_initial_width() const { return initial_width; }
  int get_intial_height() const { return initial_height; }
  double get_aspect_ratio() const { return aspect_ratio; }
  PixelFormat::Type get_format() const { return format; }
protected:
  Window(int w, int h, 
      PixelFormat::Type format, std::string_view name)
    : width{w}, height{h}, aspect_ratio{float(w)/float(h)}
    , format{format}, name{name}
    , initial_width{width}, initial_height{height}
  { }
  
  Window() = default;

  int width{0};
  int height{0};
  
  double aspect_ratio;
  PixelFormat::Type format;
  std::string_view name;

  const int initial_width{0};
  const int initial_height{0};
};

class Window_GLFW : public Window
{
public:
  Window_GLFW(const WindowParameters &params)
    : Window(params.width, params.height, PixelFormat::RGBA, params.name)
  {
    init();
    set_current();
    show();
  }
  virtual ~Window_GLFW();

  void init();
  void show();
  void hide();
  void kill();
  void set_current();
  bool is_open() const;

private:
  GLFWwindow *handle;
};

class Window_FB : public Window
{
public:
  Window_FB(std::string_view fb_file_name)
  {
    this->name = fb_file_name;
    this->format = PixelFormat::BGR;
  }

  Window_FB(const WindowParameters &params)
    : Window_FB(params.name)
  {
  }

  virtual ~Window_FB() {}

  void init() {};
  void show() {};
  void hide() {};
  void kill() {};
  void set_current() {};
  bool is_open() const { return true; };
};