#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "Window.hpp"
#include "Size.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "Screen.hpp"

class OGLRenderer : public Renderer
{
public:
  OGLRenderer();
  ~OGLRenderer();

  void set_window_current(size_t index);
  void remove_window(size_t index);
  std::shared_ptr<Window> add_window(const WindowParameters &params);

  void clear();
  void update();
  void render();

  void render_screens();

  bool should_center_view_port { true };

  void enable_blend(
    GLenum sfactor = GL_SRC_ALPHA, GLenum dfactor = GL_ONE_MINUS_SRC_ALPHA);
  void disable_blend();
  void enable_cull_face(GLenum mode = GL_FRONT, GLenum front_face = GL_CW);
  void disable_cull_face();
  void enable_depth_test(GLenum func = GL_LEQUAL);
  void disable_depth_test();

  inline void set_background_color(const Color &c)
  {
    glClearColor(c.red_float(), c.green_float(), c.blue_float(), 1.0);
  }

  inline void set_events_poll_rate(double rate) { this->poll_rate = rate; }

private:
  void generate_vertex_array_object();
  void initialize_gl();
  void uninitialize_gl();
  void center_view_port();
  GLuint vao;
  double poll_rate { 144. };
};
