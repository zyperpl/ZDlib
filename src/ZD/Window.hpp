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
  virtual void init() { printf("Window::init\n"); };
  virtual void show() {};
  virtual void hide() {};
  virtual void kill() { printf("Window::kill\n"); };
  virtual void set_current() {};
  virtual bool is_open() { return true; }

  int get_width() const { return width; }
  int get_height() const { return height; }
  double get_aspect_ratio() const { return aspect_ratio; }
  PixelFormat::Type get_format() const { return format; }

protected:
  Window(int w, int h, 
      PixelFormat::Type format, std::string_view name)
    : width{w}, height{h}, aspect_ratio{float(w)/float(h)}
    , format{format}, name{name}
  { }

  Window() = default;

  int width;
  int height;
  double aspect_ratio;
  PixelFormat::Type format;
  std::string_view name;
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
  bool is_open();

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

  void init();
  void show();
  void hide();
  void kill();
  void set_current();
  bool is_open();
};
