#include <memory>
#include <optional>

#include "3rd/glm/glm.hpp"

#include "ShaderLoader.hpp"
#include "Sprite.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "Window.hpp"

namespace ZD
{
  static const std::string_view SPRITE_RENDERER_VERTEX_SHADER = R"glsl(
  #version 330 
  precision highp float;

  in vec2 position;
  out vec2 uv;
  uniform vec2 view_size;
  uniform vec3 sprite_position = vec3(0.,0.,0.);
  uniform vec2 sprite_scale = vec2(1., 1.);
  uniform vec2 sheet_size;
  uniform vec2 frame_size;
  
  out vec2 frames_number;

  void main()
  {
    frames_number = sheet_size / frame_size;
    vec2 pixel_size = view_size / sheet_size * frames_number;
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
  uniform int frame = 0;

  out vec4 fragColor;
  void main()
  {
    vec2 suv = uv / frames_number;
    suv.x += (1.0 / frames_number.x) * frame;
    fragColor = texture(sampler, suv);

    if (fragColor.a <= 0.0) discard;
  }
)glsl";

  Sprite::Sprite(std::shared_ptr<Image> image)
  : Sprite(image, { image->width(), image->height() })
  {
  }

  Sprite::Sprite(std::shared_ptr<Image> image, const Size frame_size)
  : image { image }
  , max_frames { image->width() / frame_size.width() }
  , frame_size { frame_size }
  , model { std::make_shared<Model>(ModelDefault::Screen) }
  , texture { std::make_shared<Texture>(image) }
  , shader_program { ShaderLoader()
                       .add(SPRITE_RENDERER_VERTEX_SHADER, GL_VERTEX_SHADER)
                       .add(SPRITE_RENDERER_FRAGMENT_SHADER, GL_FRAGMENT_SHADER)
                       .compile() }
  {
  }

  Sprite::Sprite(
    std::shared_ptr<ShaderProgram> shader_program, std::shared_ptr<Image> image,
    const Size frame_size)
  : image { image }
  , max_frames { image->width() / frame_size.width() }
  , frame_size { frame_size }
  , model { std::make_shared<Model>(ModelDefault::Screen) }
  , texture { std::make_shared<Texture>(image) }
  , shader_program { shader_program }
  {
  }

  void Sprite::set_shader_uniforms(
    const RenderTarget &target, std::shared_ptr<ShaderProgram> &program)
  {
    const glm::vec2 view_size { target.get_width(), target.get_height() };
    const glm::vec3 sprite_position { position.x - view_size.x / 2.0,
                                      position.y - view_size.y / 2.0,
                                      position.z };
    const glm::vec2 f_size { frame_size.width(), frame_size.height() };
    const glm::vec2 sheet_size { image->width(), image->height() };

    program->set_uniform<glm::vec2>("view_size", view_size);
    program->set_uniform<glm::vec2>("frame_size", f_size);
    program->set_uniform<glm::vec3>("sprite_position", sprite_position);
    program->set_uniform<glm::vec2>("sprite_scale", scale);
    program->set_uniform<glm::vec2>("sheet_size", sheet_size);

    program->set_uniform("frame", frame);
  }

  void Sprite::render(const RenderTarget &target)
  {
    assert(shader_program);
    assert(model);
    assert(texture);
    assert(image);

    shader_program->use();
    set_shader_uniforms(target, shader_program);

    texture->bind(*shader_program, 0, "sprite_sheet");
    model->draw(*shader_program);
  }

} // namespace ZD
