#pragma once

#include "Texture.hpp"
#include "Model.hpp"
#include "Shader.hpp"

#include "3rd/glm/glm.hpp"

#include "Tileset.hpp"
#include "RenderTarget.hpp"

class TilesetRenderer
{
public:
  TilesetRenderer(std::shared_ptr<Tileset> tileset);
  TilesetRenderer(
    std::shared_ptr<Tileset> tileset, std::shared_ptr<ShaderProgram> shader);
  void update(const Tilemap &tilemap);
  void render(const RenderTarget &target);

  std::shared_ptr<Texture> get_tileset_texture() const { return tileset_texture; }
  std::shared_ptr<Texture> get_map_texture() const { return map_texture; }

  glm::vec3 position { 0., 0., 0. };
  glm::vec2 scale { 1., 1. };
  glm::vec2 view_scale { 1., 1. };
  glm::vec2 view_offset { 0., 0. };

  static size_t MAP_TEXTURE_WIDTH;
  static size_t MAP_TEXTURE_HEIGHT;
private:
  std::shared_ptr<Tileset> tileset;
  std::shared_ptr<ShaderProgram> shader_program;
  std::shared_ptr<Texture> tileset_texture;
  std::shared_ptr<Texture> map_texture;
  std::shared_ptr<Model> model;
};
