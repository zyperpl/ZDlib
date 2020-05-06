#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "RenderTarget.hpp"
#include "Texture.hpp"

class FramebufferObject : public RenderTarget
{
public:
  std::shared_ptr<Texture> texture;
  int get_width() const { return width; }
  int get_height() const { return height; }
  size_t width { 0 }, height { 0 };
private:
  GLuint id { 0 };
  GLuint renderbuffer_id { 0 };

  friend class OGLRenderer;
};
