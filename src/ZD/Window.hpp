#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Color.hpp"
#include "Size.hpp"
#include "RenderTarget.hpp"

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

class Window : public RenderTarget
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
  int get_window_width() const { return render_width; }
  int get_window_height() const { return render_height; }
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
  , name { name }
  , render_width { width }
  , render_height { height }
  , aspect_ratio { float(w) / float(h) }
  , format { format }
  {
  }

  Window() = default;

  int width { 0 }; // pixel buffer width
  int height { 0 }; // pixel buffer height

  std::string_view name;

  int render_width { 0 }; // framebuffer/window width
  int render_height { 0 }; // framebuffer/window height

  double aspect_ratio;
  PixelFormat::Type format;

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
  bool should_center_view_port { true };

private:
  GLFWwindow *handle;
  std::shared_ptr<Input_GLFW> input_ptr;
  void set_framebuffer_size(int w, int h)
  {
    this->render_width = w;
    this->render_height = h;

    if (should_center_view_port)
    {
      center_view_port();
    }
    else
    {
      glViewport(0, 0, w, h);
    }
  }
  void center_view_port();

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
