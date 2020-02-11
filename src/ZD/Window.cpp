#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Window.hpp"

Window_GLFW::~Window_GLFW()
{
  puts("~WINDOW_GLFW");
  this->kill();
}

void Window_GLFW::init()
{
  handle = glfwCreateWindow(width, height, name.data(), NULL, NULL);
  assert(handle != NULL);
  printf("Window_GLFW (%p) created.\n", handle);
}

void Window_GLFW::show()
{
  glfwShowWindow(handle);
}

void Window_GLFW::hide()
{
  glfwHideWindow(handle);
}

void Window_GLFW::kill()
{
  glfwDestroyWindow(handle);
  puts("GLFW window destroyed.");
}

void Window_GLFW::set_current()
{
  assert(handle != NULL);
  glfwMakeContextCurrent(handle);
}

bool Window_GLFW::is_open() const
{
  return !glfwWindowShouldClose(handle);
}
