#pragma once

#include "Image.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "RenderTarget.hpp"

#include <memory>

#include "3rd/glm/glm.hpp"

class Sprite
{
public:
  Sprite(std::shared_ptr<Image> image, const Size frame_size);
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

  int get_frame() const { return frame; }
  int get_max_frames() const { return max_frames; }

  virtual void render(const RenderTarget&);

  std::shared_ptr<Image> image;

  glm::vec3 position { 0., 0., 0. };
  glm::vec2 scale { 1., 1. };

protected:
  int frame { 0 };
  const int max_frames { 1 };
  const Size frame_size { 0, 0 };

private:
  std::shared_ptr<Texture> texture;
  std::shared_ptr<Model> model;
  std::shared_ptr<ShaderProgram> shader_program;
};
