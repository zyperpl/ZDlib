#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Color.hpp"
#include "Size.hpp"

class Input;
class Input_GLFW;
class Screen;

struct WindowParameters
{
  int width { 0 };
  int height { 0 };
  std::string_view name { "" };

  WindowParameters(Size size, std::string_view name)
  : width { size.width() }
  , height { size.height() }
  , name { name }
  {
  }

  WindowParameters(std::string_view name)
  : name { name }
  {
  }
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
  int get_view_width() const { return view_width; }
  int get_view_height() const { return view_height; }
  double get_aspect_ratio() const { return aspect_ratio; }
  PixelFormat::Type get_format() const { return format; }

  virtual void add_screen(std::shared_ptr<Screen> screen)
  {
    screens.push_back(screen);
  }
  std::vector<std::shared_ptr<Screen>> get_screens() { return screens; }

  virtual const Input *input() const = 0;

protected:
  Window(int w, int h, PixelFormat::Type format, std::string_view name)
  : width { w }
  , height { h }
  , aspect_ratio { float(w) / float(h) }
  , format { format }
  , name { name }
  , view_width { width }
  , view_height { height }
  {
  }

  Window() = default;

  int width { 0 };
  int height { 0 };

  double aspect_ratio;
  PixelFormat::Type format;
  std::string_view name;

  const int view_width { 0 };
  const int view_height { 0 };
  std::vector<std::shared_ptr<Screen>> screens;
};

class Window_GLFW : public Window
{
public:
  Window_GLFW(const WindowParameters &params)
  : Window(params.width, params.height, PixelFormat::RGB, params.name)
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
  const Input *input() const;

private:
  GLFWwindow *handle;
  std::shared_ptr<Input_GLFW> input_ptr;
  void set_framebuffer_size(int width, int height)
  {
    this->width = width;
    this->height = height;
    if (should_center_view_port)
    {
      center_view_port();
    }
  }
  void center_view_port();
  bool should_center_view_port { true };

  friend void key_callback_glfw(GLFWwindow *, int, int, int, int);
  friend void cursor_position_callback_glfw(GLFWwindow *, double, double);
  friend void mouse_button_callback_glfw(GLFWwindow *, int, int, int);
  friend void framebuffer_size_callback_glfw(GLFWwindow *, int, int);
  friend void mouse_scroll_callback_glfw(GLFWwindow *, double, double);
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
  const Input *input() const { return input_ptr.get(); }

private:
  std::shared_ptr<Input> input_ptr;
};
