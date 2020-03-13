#pragma once
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Image.hpp"
#include "Shader.hpp"

struct TextureWrap
{
  float x { 1 }, y { 1 };
};

struct TextureParameters
{
  TextureWrap wrap { 1.0f, 1.0f };
  bool generate_mipmap { false };
  int mag_filter { GL_NEAREST };
  int min_filter { GL_NEAREST };
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
  void generate(TextureParameters params);
  bool set_uniform(const ShaderUniform &uniform);

  std::shared_ptr<Image> image;
  TextureWrap texture_wrap { 1.0f, 1.0f };
  bool generate_mipmap { false };

  GLuint id { 0 };
  GLuint sampler_id { 0 };
};
