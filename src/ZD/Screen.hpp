#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Image.hpp"
#include "Texture.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Painter.hpp"

class Screen
{
public:
  Screen(int x, int y, int width, int height);
  std::shared_ptr<Image> image() { return canvas_image; }
  std::shared_ptr<Painter> painter() { return image_painter; }
  virtual void render() = 0;
  mutable bool rendered { false };
protected:
  int x, y, width, height;
  std::shared_ptr<Image> canvas_image;
  std::shared_ptr<Painter> image_painter;
};

class Screen_GL : public Screen
{
public:
  Screen_GL(
    std::shared_ptr<ShaderProgram> shader, int x, int y, int width, int height);
  Screen_GL(int x, int y, int width, int height);
  void render();

private:
  std::unique_ptr<Texture> texture;
  std::unique_ptr<Model> model;
  std::shared_ptr<ShaderProgram> shader_program;

  friend class OGLRenderer;
};
