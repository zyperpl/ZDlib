#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "TilesetRenderer.hpp"
#include "ShaderLoader.hpp"

namespace ZD
{
  size_t TilesetRenderer::MAP_TEXTURE_WIDTH = 256;
  size_t TilesetRenderer::MAP_TEXTURE_HEIGHT = 256;

  static const GLchar *TILESET_RENDERER_VERTEX_SHADER = R"glsl(
  #version 330 
  precision highp float;

  in vec2 position;
  out vec2 map_uv;
  out vec2 screen_uv;
  uniform vec2 view_size;
  uniform vec2 view_scale;
  uniform vec2 view_offset;
  uniform vec3 screen_position;
  uniform vec2 screen_scale;
  uniform vec2 texture_size;
  uniform vec2 tile_size;
  
  void main()
  {
    vec2 pixel_size = view_size / texture_size * screen_scale;
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    vec2 uv = gl_Position.xy / 2.0 + 0.5;
    uv.y = 1.0 - uv.y;
    screen_uv = uv * view_size / view_scale + view_offset;
    screen_uv += pixel_size * 0.045;
    map_uv = screen_uv / texture_size / tile_size;
    gl_Position.xy *= screen_scale;
    gl_Position.xy += (vec2(screen_position.x, -screen_position.y) / view_size) * 2.;
    gl_Position.z = -screen_position.z;
  }
)glsl";

  static const GLchar *TILESET_RENDERER_FRAGMENT_SHADER = R"glsl(
  #version 330 

  in vec2 map_uv;
  in vec2 screen_uv;
  uniform sampler2D tileset_sampler;
  uniform sampler2D map_sampler;
  uniform vec2 spritesheet_size;
  uniform vec2 tile_size;

  out vec4 fragColor;
  void main()
  {
    if (screen_uv.x < 0.0 || screen_uv.y < 0.0) { discard; }
    vec2 uv = texture(map_sampler, map_uv).rg;
    if (uv.x == 1.0 && uv.y == 1.0) { discard; }
    vec2 spriteOffset = floor(uv * 256.0) * tile_size;
    vec2 spriteCoord = mod(screen_uv, tile_size);
    fragColor = texture(tileset_sampler, (spriteOffset + spriteCoord) / spritesheet_size);

    if (fragColor.a <= 0.0) discard;
  }
)glsl";

  TilesetRenderer::TilesetRenderer(std::shared_ptr<Tileset> tileset)
  : tileset { tileset }
  {
    shader_program = ShaderLoader()
                       .add(TILESET_RENDERER_FRAGMENT_SHADER, GL_FRAGMENT_SHADER)
                       .add(TILESET_RENDERER_VERTEX_SHADER, GL_VERTEX_SHADER)
                       .compile();

    map_texture = Texture::create();
    model = Model::load(ModelDefault::Screen);
    tileset_texture = Texture::load(tileset->source);
  }

  TilesetRenderer::TilesetRenderer(std::shared_ptr<Tileset> tileset, std::shared_ptr<ShaderProgram> shader)
  : tileset { tileset }
  , shader_program { shader }
  {
    map_texture = Texture::create();
    model = Model::load(ModelDefault::Screen);
    tileset_texture = Texture::load(tileset->source);
  }

  void TilesetRenderer::update(const Tilemap &tilemap)
  {
    std::shared_ptr<Image> image =
      Image::create(Size(MAP_TEXTURE_WIDTH, MAP_TEXTURE_HEIGHT), Color(255, 255, 255), PixelFormat::RGBA);
    for (const auto &key_tile : tilemap.get_tiles())
    {
      const auto &tile = key_tile.second;
      Color c(tile.index.x, tile.index.y, 0);
      image->set_pixel(tile.position.x, tile.position.y, c);
    }
    map_texture->set_image(image);
  }

  void TilesetRenderer::render(const RenderTarget &target)
  {
    if (!map_texture->get_image())
      return;

    shader_program->use();

    shader_program->set_uniform<glm::vec2>("view_offset", view_offset);
    shader_program->set_uniform<glm::vec2>("view_scale", view_scale);
    shader_program->set_uniform<glm::vec2>("view_size", { target.get_width(), target.get_height() });
    shader_program->set_uniform<glm::vec3>("screen_position", position);
    shader_program->set_uniform<glm::vec2>("screen_scale", scale);
    shader_program->set_uniform<glm::vec2>(
      "texture_size", { map_texture->get_image()->width(), map_texture->get_image()->height() });
    shader_program->set_uniform<glm::vec2>(
      "spritesheet_size", { tileset_texture->get_image()->width(), tileset_texture->get_image()->height() });

    shader_program->set_uniform<glm::vec2>("tile_size", { tileset->get_tile_width(), tileset->get_tile_height() });

    tileset_texture->bind(*shader_program, 0, "tileset_sampler");
    map_texture->bind(*shader_program, 1, "map_sampler");
    model->draw(*shader_program);
  }

} // namespace ZD
