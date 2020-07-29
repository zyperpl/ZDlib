#include <memory>

#include "Screen.hpp"
#include "ShaderLoader.hpp"

#include "3rd/glm/glm.hpp"

namespace ZD
{
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
    std::shared_ptr<Texture> texture, int x, int y, int width, int height)
  : Screen(x, y, width, height)
  , texture { texture }
  {
    model = std::make_unique<Model>(ModelDefault::Screen);

    shader_program =
      ShaderLoader()
        .add(ShaderDefault::ScreenTextureVertex, GL_VERTEX_SHADER)
        .add(ShaderDefault::ScreenTextureFragment, GL_FRAGMENT_SHADER)
        .compile();
  }

  Screen_GL::Screen_GL(
    std::shared_ptr<ShaderProgram> shader, int x, int y, int width, int height)
  : Screen(x, y, width, height)
  , shader_program { shader }
  {
    texture = std::make_shared<Texture>(canvas_image);
    model = std::make_unique<Model>(ModelDefault::Screen);
  }

  Screen_GL::Screen_GL(int x, int y, int width, int height)
  : Screen(x, y, width, height)
  {
    texture = std::make_shared<Texture>(canvas_image);
    model = std::make_unique<Model>(ModelDefault::Screen);

    shader_program =
      ShaderLoader()
        .add(ShaderDefault::ScreenTextureVertex, GL_VERTEX_SHADER)
        .add(ShaderDefault::ScreenTextureFragment, GL_FRAGMENT_SHADER)
        .compile();
  }

  void Screen_GL::render(const RenderTarget &target)
  {
    shader_program->use();

    shader_program->set_uniform<glm::vec2>(
      "view_size", { target.get_width(), target.get_height() });
    shader_program->set_uniform<glm::vec2>("screen_position", { x, y });
    shader_program->set_uniform<glm::vec2>(
      "screen_scale", { scale.x, scale.y });
    shader_program->set_uniform<int>("flip_y", flip_y);

    //printf("image=%p changes=%d\n", &canvas_image, canvas_image->change_counter());
    if (canvas_image && canvas_image->is_changed())
    {
      if (texture)
      {
        texture->update();
      }
      canvas_image->reset_change_counter();
    }

    if (!enabled)
      return;

    shader_program->set_uniform<glm::vec2>(
      "texture_size", { texture->get_width(), texture->get_height() });

    if (texture)
    {
      texture->bind(*shader_program);
    }
    if (model)
    {
      model->draw(*shader_program);
    }
  }

} // namespace ZD
