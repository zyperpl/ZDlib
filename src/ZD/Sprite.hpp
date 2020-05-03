#pragma once

#include "Image.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "RenderTarget.hpp"

#include <memory>

class SpriteRenderer;

class Sprite
{
public:
  Sprite(
    std::shared_ptr<Image> image, const int image_width, const int image_height,
    int starting_frame = 0);
  virtual ~Sprite() {};

  int set_frame(int new_frame)
  {
    frame = new_frame % max_frames;
    return frame;
  }
  int increase_frame(int amount = 1)
  {
    frame += amount;
    frame %= max_frames;
    return frame;
  }
  int decrease_frame(int amount = 1)
  {
    frame -= amount;
    frame %= max_frames;
    return frame;
  }

  void render(const RenderTarget &target);
  void set_renderer(std::shared_ptr<SpriteRenderer> renderer);

  int x { 0 }, y { 0 };
private:
  std::shared_ptr<SpriteRenderer> renderer;
  int frame { 0 };
  const int max_frames { 1 };
};

class SpriteRenderer
{
public:
  static std::shared_ptr<SpriteRenderer> create(
    std::shared_ptr<Image> source, int image_width, int image_height);
  static std::shared_ptr<SpriteRenderer> create(
    std::shared_ptr<Image> source, std::shared_ptr<ShaderProgram> program,
    int image_width, int image_height);

  void set_shader(std::shared_ptr<ShaderProgram> program)
  {
    shader_program = program;
  }
  std::shared_ptr<ShaderProgram> get_shader_program() const
  {
    return shader_program;
  }

  void set_sheet(std::shared_ptr<Image> source) { sheet_image = source; }
  std::shared_ptr<Image> get_sheet() const { return sheet_image; }

  void render(const RenderTarget &target, int x, int y, int frame = 0);

private:
  SpriteRenderer(
    std::shared_ptr<Image> source, std::shared_ptr<ShaderProgram> program,
    int image_width, int image_height);
  std::shared_ptr<Image> sheet_image;
  std::shared_ptr<Texture> sheet_texture;
  std::shared_ptr<Model> model;
  std::shared_ptr<ShaderProgram> shader_program;
  int image_width { 0 };
  int image_height { 0 };
};
