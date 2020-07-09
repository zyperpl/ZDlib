#include <memory>
#include <optional>

#include "3rd/glm/glm.hpp"

#include "ShaderLoader.hpp"
#include "Sprite.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "Window.hpp"

static const std::string_view SPRITE_RENDERER_VERTEX_SHADER = R"glsl(
  #version 330 
  precision highp float;

  in vec2 position;
  out vec2 uv;
  uniform vec2 view_size;
  uniform vec3 sprite_position = vec3(0.,0.,0.);
  uniform vec2 sprite_scale;
  uniform vec2 texture_size;
  uniform vec2 sprite_size;
  
  out vec2 frames_number;

  void main()
  {
    frames_number = texture_size / sprite_size;
    vec2 pixel_size = view_size / texture_size * frames_number;
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    uv = gl_Position.xy / 2.0 + 0.5;
    uv.y = 1.0 - uv.y;
    uv += pixel_size * 0.000002;
    gl_Position.xy /= pixel_size;
    gl_Position.xy *= sprite_scale;
    gl_Position.xy += (vec2(sprite_position.x, -sprite_position.y) / view_size) * 2.;
    gl_Position.z = -sprite_position.z;
  }
)glsl";

static const std::string_view SPRITE_RENDERER_FRAGMENT_SHADER = R"glsl(
  #version 330 

  in vec2 uv;
  in vec2 frames_number;
  
  uniform sampler2D sampler;
  uniform vec2 sprite_size;
  uniform int frame;
  uniform vec2 texture_size;

  out vec4 fragColor;
  void main()
  {
    vec2 suv = uv / frames_number;
    suv.x += (1.0 / frames_number.x) * frame;
    fragColor = texture(sampler, suv);

    if (fragColor.a <= 0.0) discard;
  }
)glsl";

Sprite::Sprite(
  std::shared_ptr<Image> image, const int image_width, const int image_height,
  int starting_frame)
: renderer { SpriteRenderer::create(image, image_width, image_height) }
, frame { starting_frame }
, max_frames { image->width() / image_width }
{
}

void Sprite::set_renderer(std::shared_ptr<SpriteRenderer> renderer)
{
  this->renderer = renderer;
}

void Sprite::render(const RenderTarget &target)
{
  renderer->render(target, position, scale, frame);
}

std::shared_ptr<SpriteRenderer> SpriteRenderer::create(
  std::shared_ptr<Image> source, int image_width, int image_height)
{
  std::shared_ptr<ShaderProgram> shader_program =
    ShaderLoader()
      .add(SPRITE_RENDERER_VERTEX_SHADER, GL_VERTEX_SHADER)
      .add(SPRITE_RENDERER_FRAGMENT_SHADER, GL_FRAGMENT_SHADER)
      .compile();
  return std::shared_ptr<SpriteRenderer>(
    new SpriteRenderer(source, shader_program, image_width, image_height));
}

std::shared_ptr<SpriteRenderer> SpriteRenderer::create(
  std::shared_ptr<Image> source, std::shared_ptr<ShaderProgram> shader_program,
  int image_width, int image_height)
{
  return std::shared_ptr<SpriteRenderer>(
    new SpriteRenderer(source, shader_program, image_width, image_height));
}

SpriteRenderer::SpriteRenderer(
  std::shared_ptr<Image> source, std::shared_ptr<ShaderProgram> program,
  int image_width, int image_height)
: sheet_image { source }
, shader_program { program }
, image_width { image_width }
, image_height { image_height }
{
  model = std::make_shared<Model>(ModelDefault::Screen);
  sheet_texture = std::make_shared<Texture>(sheet_image);
}

void SpriteRenderer::render(
  const RenderTarget &target, const glm::vec3 &position, const glm::vec2 &scale,
  int frame)

{
  shader_program->use();

  const glm::vec2 view_size { target.get_width(), target.get_height() };
  const glm::vec3 sprite_position { position.x - view_size.x / 2.0,
                                    position.y - view_size.y / 2.0,
                                    position.z };

  shader_program->set_uniform<glm::vec2>("view_size", view_size);
  shader_program->set_uniform<glm::vec2>(
    "sprite_size", { image_width, image_height });
  shader_program->set_uniform<glm::vec3>("sprite_position", sprite_position);
  shader_program->set_uniform<glm::vec2>("sprite_scale", scale);
  shader_program->set_uniform<glm::vec2>(
    "texture_size", { sheet_image->width(), sheet_image->height() });

  shader_program->set_uniform("frame", frame);

  sheet_texture->bind(*shader_program, 0, "sprite_sheet");
  model->draw(*shader_program);
}
