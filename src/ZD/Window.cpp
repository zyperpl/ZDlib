#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Window.hpp"

Window_GLFW::~Window_GLFW()
{
  this->kill();
}

void Window_GLFW::init()
{
  handle = glfwCreateWindow(width, height, name.data(), NULL, NULL);
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
}

void Window_GLFW::set_current()
{
  glfwMakeContextCurrent(handle);
}

bool Window_GLFW::is_open()
{
  return !glfwWindowShouldClose(handle);
}
