#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Image.hpp"
#include "Texture.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Painter.hpp"
#include "Window.hpp"

struct ScreenScale
{
  ScreenScale() = default;

  ScreenScale(float x, float y)
  : x { x }
  , y { y }
  {
  }

  float x { 1.0f };
  float y { 1.0f };
};

class Screen
{
public:
  Screen(int x, int y, int width, int height);
  virtual ~Screen() = default;
  std::shared_ptr<Image> image() { return canvas_image; }
  std::shared_ptr<Painter> painter() { return image_painter; }
  virtual const Input *input() = 0;
  virtual void render(Window &window) = 0;
  int center_x() const { return x + width / 2; }
  int center_y() const { return y + height / 2; }
  int left() const { return x; }
  int right() const { return x + width; }
  int top() const { return y; }
  int bottom() const { return y + height; }
  int x { 0 }, y { 0 }, width, height;
  ScreenScale scale;

  bool flip_y { false };
protected:
  std::shared_ptr<Image> canvas_image;
  std::shared_ptr<Painter> image_painter;
};

class Screen_GL : public Screen
{
public:
  Screen_GL(std::shared_ptr<Texture> texture, int x, int y, int width, int height);
  Screen_GL(
    std::shared_ptr<ShaderProgram> shader, int x, int y, int width, int height);
  Screen_GL(int x, int y, int width, int height);
  const Input *input();
  void render(Window &window);

private:
  std::shared_ptr<Input_GLFW> input_gl;
  std::shared_ptr<Texture> texture;
  std::unique_ptr<Model> model;
  std::shared_ptr<ShaderProgram> shader_program;

  friend class OGLRenderer;
  friend void key_callback_glfw(GLFWwindow *, int, int, int, int);
  friend void cursor_position_callback_glfw(GLFWwindow *, double, double);
  friend void mouse_button_callback_glfw(GLFWwindow *, int, int, int);
  friend void window_size_callback_glfw(GLFWwindow *, int, int);
  friend void mouse_scroll_callback_glfw(GLFWwindow *, double, double);
};
