#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <unordered_map>

#include "Window.hpp"

static std::unordered_map<GLFWwindow*, Window_GLFW*> windows_GLFW;

void key_callback_glfw(GLFWwindow* handle, int key, int, int action, int)
{
  Input_GLFW* input = windows_GLFW.at(handle)->input_ptr.get();
  input->update_key(key, action);
}

void cursor_position_callback_glfw(GLFWwindow* handle, double xpos, double ypos)
{
  auto window = windows_GLFW.at(handle);
  Input_GLFW* input = window->input_ptr.get();
  Size window_size { window->get_width(), window->get_height() };
  Size initial_size { window->get_initial_width(),
                      window->get_intial_height() };
  input->update_mouse_position(xpos, ypos, window_size, initial_size);
}

void mouse_button_callback_glfw(GLFWwindow* handle, int button, int action, int)
{
  auto window = windows_GLFW.at(handle);
  Input_GLFW* input = window->input_ptr.get();
  input->update_mouse_button(button, action);
}

void window_size_callback_glfw(GLFWwindow* handle, int width, int height)
{
  auto window = windows_GLFW.at(handle);
  window->set_size(width, height);
}

Window_GLFW::~Window_GLFW()
{
  if (handle != NULL) { this->kill(); }
}

void Window_GLFW::init()
{
  handle = glfwCreateWindow(width, height, name.data(), NULL, NULL);
  assert(handle != NULL);
  printf("Window_GLFW (%p) created.\n", handle);
  glfwSetInputMode(handle, GLFW_STICKY_KEYS, GLFW_TRUE);

  input_ptr = std::make_unique<Input_GLFW>();

  windows_GLFW.insert({ handle, this });
  glfwSetKeyCallback(handle, key_callback_glfw);
  glfwSetCursorPosCallback(handle, cursor_position_callback_glfw);
  glfwSetMouseButtonCallback(handle, mouse_button_callback_glfw);
  glfwSetWindowSizeCallback(handle, window_size_callback_glfw);
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
  glfwDestroyWindow(handle);
  handle = NULL;
  puts("GLFW window destroyed.");
}

void Window_GLFW::set_current()
{
  assert(handle != NULL);
  glfwMakeContextCurrent(handle);
}

bool Window_GLFW::is_open() const
{
  if (handle == NULL) return false;

  return !glfwWindowShouldClose(handle);
}
