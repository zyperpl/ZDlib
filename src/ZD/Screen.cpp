#include <memory>

#include "Screen.hpp"
#include "ShaderLoader.hpp"

Screen::Screen(int x, int y, int width, int height)
: x { x }
, y { y }
, width { width }
, height { height }
, canvas_image { Image::create(Size(width, height), PixelFormat::RGBA) }
, image_painter { std::make_shared<Painter>(canvas_image) }
{
}

Screen_GL::Screen_GL(
  std::shared_ptr<ShaderProgram> shader, int x, int y, int width, int height)
: Screen(x, y, width, height)
, shader_program { shader }
{
  texture = std::make_unique<Texture>(canvas_image);
  model = std::make_unique<Model>(ModelDefault::Screen);
}

Screen_GL::Screen_GL(int x, int y, int width, int height)
: Screen(x, y, width, height)
{
  texture = std::make_unique<Texture>(canvas_image);
  model = std::make_unique<Model>(ModelDefault::Screen);

  shader_program =
    ShaderLoader()
      .add(ShaderDefault::ScreenTextureVertex, GL_VERTEX_SHADER)
      .add(ShaderDefault::ScreenTextureFragment, GL_FRAGMENT_SHADER)
      .compile();
}

void Screen_GL::render()
{
  shader_program->use();

  texture->update();
  texture->bind(*shader_program);

  model->draw(*shader_program);

  rendered = true;
}