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
  Texture(TextureParameters params = {});
  Texture(std::shared_ptr<Image> image);
  Texture(std::shared_ptr<Image> image, TextureParameters params);
  virtual ~Texture();

  void update();
  void bind(
    const ShaderProgram &shader, GLuint sampler_id = 0,
    std::string_view sampler_name = "sampler");

  void set_image(std::shared_ptr<Image> new_image);
  const std::shared_ptr<Image> get_image() const { return this->image; }

private:
  void generate(TextureParameters params);
  void set_buffer_data();
  bool set_uniform(const ShaderUniform &uniform);

  std::shared_ptr<Image> image;
  TextureWrap texture_wrap { 1.0f, 1.0f };
  bool generate_mipmap { false };

  GLuint id { 0 };
  GLuint pbo[2] { 0, 0 };
  long frame { 0 };
};
