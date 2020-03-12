#include <memory>

#include "Screen.hpp"
#include "ShaderLoader.hpp"
#include "Input.hpp"
#include "ScreenInput.hpp"

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
  auto screen_input = std::make_shared<ScreenInput>(*this);
  input_gl = std::make_shared<Input_GLFW>(screen_input);
  texture = std::make_unique<Texture>(canvas_image);
  model = std::make_unique<Model>(ModelDefault::Screen);
}

Screen_GL::Screen_GL(int x, int y, int width, int height)
: Screen(x, y, width, height)
{
  auto screen_input = std::make_shared<ScreenInput>(*this);
  input_gl = std::make_shared<Input_GLFW>(screen_input);
  texture = std::make_unique<Texture>(canvas_image);
  model = std::make_unique<Model>(ModelDefault::Screen);

  shader_program =
    ShaderLoader()
      .add(ShaderDefault::ScreenTextureVertex, GL_VERTEX_SHADER)
      .add(ShaderDefault::ScreenTextureFragment, GL_FRAGMENT_SHADER)
      .compile();
}

void Screen_GL::render(Window &window)
{
  shader_program->use();

  shader_program->set_uniform<glm::vec2>(
    "view_size", { window.get_view_width(), window.get_view_height() });
  shader_program->set_uniform<glm::vec2>(
    "frambuffer_size", { window.get_width(), window.get_height() });
  shader_program->set_uniform<glm::vec2>("screen_position", { x, y });
  shader_program->set_uniform<glm::vec2>("screen_scale", { scale.x, scale.y });

  //printf("image=%p changes=%d\n", &canvas_image, canvas_image->change_counter());
  if (canvas_image->is_changed())
  {
    texture->update();
  }
  shader_program->set_uniform<glm::vec2>(
    "texture_size",
    { texture->get_image()->width(), texture->get_image()->height() });

  texture->bind(*shader_program);
  model->draw(*shader_program);
  canvas_image->reset_change_counter();
}

const Input *Screen_GL::input() { return input_gl->get(); }
