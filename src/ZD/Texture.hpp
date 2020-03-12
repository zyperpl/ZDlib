#pragma once
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Image.hpp"
#include "Shader.hpp"

struct TextureWrap
{
  float x, y;
};

struct TextureParameters
{
  TextureWrap wrap;
};

class Texture
{
public:
  Texture(std::shared_ptr<Image> image);
  Texture(std::shared_ptr<Image> image, TextureParameters params);

  void update();
  void update(Image *new_image);

  void bind(const ShaderProgram &shader);
  const std::shared_ptr<Image> get_image() const { return this->image; }
private:
  void generate();
  bool set_uniform(const ShaderUniform &uniform);

  std::shared_ptr<Image> image;
  TextureWrap texture_wrap { 1.0f, 1.0f };

  GLuint id { 0 };
  GLuint sampler_id { 0 };
};
