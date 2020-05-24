#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "TilesetRenderer.hpp"
#include "ShaderLoader.hpp"
  
size_t TilesetRenderer::MAP_TEXTURE_WIDTH = 256;
size_t TilesetRenderer::MAP_TEXTURE_HEIGHT = 256;

TilesetRenderer::TilesetRenderer(std::shared_ptr<Tileset> tileset)
: tileset { tileset }
{
  shader_program = ShaderLoader()
                     .add(ShaderDefault::TilesetFragment, GL_FRAGMENT_SHADER)
                     .add(ShaderDefault::TilesetVertex, GL_VERTEX_SHADER)
                     .compile();

  map_texture = std::make_shared<Texture>();
  model = std::make_shared<Model>(ModelDefault::Screen);
  tileset_texture = std::make_shared<Texture>(tileset->source);
}

TilesetRenderer::TilesetRenderer(
  std::shared_ptr<Tileset> tileset, std::shared_ptr<ShaderProgram> shader)
: tileset { tileset }
, shader_program { shader }
{
  map_texture = std::make_shared<Texture>();
  model = std::make_shared<Model>(ModelDefault::Screen);
  tileset_texture = std::make_shared<Texture>(tileset->source);
}

void TilesetRenderer::update(const Tilemap &tilemap)
{
  std::shared_ptr<Image> image = Image::create(
    Size(MAP_TEXTURE_WIDTH, MAP_TEXTURE_HEIGHT),
    Color(255, 255, 255),
    PixelFormat::RGBA);
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
  shader_program->set_uniform<glm::vec2>(
    "view_size", { target.get_width(), target.get_height() });
  shader_program->set_uniform<glm::vec2>("screen_position", position);
  shader_program->set_uniform<glm::vec2>("screen_scale", scale);
  shader_program->set_uniform<glm::vec2>(
    "texture_size",
    { map_texture->get_image()->width(), map_texture->get_image()->height() });
  shader_program->set_uniform<glm::vec2>(
    "spritesheet_size",
    { tileset_texture->get_image()->width(),
      tileset_texture->get_image()->height() });

  shader_program->set_uniform<glm::vec2>(
    "tile_size", { tileset->get_tile_width(), tileset->get_tile_height() });

  tileset_texture->bind(*shader_program, 0, "tileset_sampler");
  map_texture->bind(*shader_program, 1, "map_sampler");
  model->draw(*shader_program);
}
