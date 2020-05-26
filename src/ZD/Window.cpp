#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <unordered_map>

#include "Window.hpp"
#include "Input.hpp"
#include "Screen.hpp"

static std::unordered_map<GLFWwindow*, Window_GLFW*> windows_GLFW;

void key_callback_glfw(GLFWwindow* handle, int key, int, int action, int)
{
  auto window = windows_GLFW.at(handle);
  Input_GLFW* input = window->input_ptr.get();
  input->update_key(key, action);
}

void cursor_position_callback_glfw(GLFWwindow* handle, double xpos, double ypos)
{
  auto window = windows_GLFW.at(handle);
  Input_GLFW* input = window->input_ptr.get();
  Size window_size { window->get_window_width(), window->get_window_height() };
  Size view_size { window->get_width(), window->get_height() };
  input->update_mouse_position(xpos, ypos, window_size, view_size);
}

void mouse_button_callback_glfw(GLFWwindow* handle, int button, int action, int)
{
  auto window = windows_GLFW.at(handle);
  Input_GLFW* input = window->input_ptr.get();
  input->update_mouse_button(button, action);
}

void framebuffer_size_callback_glfw(GLFWwindow* handle, int width, int height)
{
  auto window = windows_GLFW.at(handle);
  window->set_framebuffer_size(width, height);
}

void mouse_scroll_callback_glfw(
  GLFWwindow* handle, double x_offset, double y_offset)
{
  auto window = windows_GLFW.at(handle);
  Input_GLFW* input = window->input_ptr.get();
  input->add_mouse_scroll(x_offset, y_offset);
}

Window_GLFW::~Window_GLFW()
{
  if (handle != NULL) { this->kill(); }
  puts("Window destroyed.");
}

void Window_GLFW::init()
{
  printf(
    "Creating GLFW window handle [%d, %d, %s]...\n",
    width,
    height,
    name.data());
  handle = glfwCreateWindow(width, height, name.data(), NULL, NULL);
  assert(handle != NULL);
  glfwSetInputMode(handle, GLFW_STICKY_KEYS, GLFW_TRUE);

  input_ptr = std::make_shared<Input_GLFW>();

  windows_GLFW.insert({ handle, this });
  glfwSetKeyCallback(handle, key_callback_glfw);
  glfwSetCursorPosCallback(handle, cursor_position_callback_glfw);
  glfwSetMouseButtonCallback(handle, mouse_button_callback_glfw);
  glfwSetFramebufferSizeCallback(handle, framebuffer_size_callback_glfw);
  glfwSetScrollCallback(handle, mouse_scroll_callback_glfw);
}

void Window_GLFW::show() { glfwShowWindow(handle); }

void Window_GLFW::hide() { glfwHideWindow(handle); }

void Window_GLFW::kill()
{
  windows_GLFW.erase(handle);
  glfwSetKeyCallback(handle, NULL);
  glfwSetCursorPosCallback(handle, NULL);
  glfwSetMouseButtonCallback(handle, NULL);

  glfwSetWindowSizeCallback(handle, NULL);
  glfwSetScrollCallback(handle, NULL);
  glfwDestroyWindow(handle);
  handle = NULL;
}

void Window_GLFW::set_current()
{
  assert(handle != NULL);
  glfwMakeContextCurrent(handle);
  set_framebuffer_size(render_width, render_height);
}

bool Window_GLFW::is_open() const
{
  if (handle == NULL) return false;

  return !glfwWindowShouldClose(handle);
}

const Input* Window_GLFW::input() const { return input_ptr.get(); }

void Window_GLFW::center_view_port()
{
  const double desirable_width = get_width();
  const double desirable_height = get_height();
  
  double r = double(render_width) / desirable_width;
  const double r2 = double(render_height) / desirable_height;

  if (r > r2) r = r2;
  const int w = r * desirable_width;
  const int h = r * desirable_height;
  glViewport((render_width - w) / 2, (render_height - h) / 2, w, h);
}
