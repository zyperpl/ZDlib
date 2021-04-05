#pragma once
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Image.hpp"
#include "Shader.hpp"
#include "File.hpp"

namespace ZD
{
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
    int wrap_mode { GL_CLAMP_TO_EDGE };
  };

  class Texture
  {
  public:
    static std::shared_ptr<Texture> create(const TextureParameters params = TextureParameters {});
    static std::shared_ptr<Texture> load(
      std::shared_ptr<Image> image, const TextureParameters params = TextureParameters {},
      ForceReload reload = ForceReload::No);
    static std::shared_ptr<Texture> load(
      const std::string image_name, const TextureParameters params = TextureParameters {},
      ForceReload reload = ForceReload::No);

    virtual ~Texture();

    void update();
    void bind(const ShaderProgram &shader, GLuint sampler_id = 0, std::string_view sampler_name = "sampler");

    void set_name(const std::string name) { this->name = name; }
    void set_image(std::shared_ptr<Image> new_image);

    GLuint get_id() const { return id; }
    const std::string &get_name() const { return name; }
    const std::shared_ptr<Image> get_image() const { return this->image; }
    int get_width(int mip_level = 0);
    int get_height(int mip_level = 0);

  private:
    Texture(const TextureParameters params = TextureParameters {});
    Texture(const std::shared_ptr<Image> image, const TextureParameters params = TextureParameters {});

    void generate(const TextureParameters params);
    void set_buffer_data();
    bool set_uniform(const ShaderUniform &uniform);

    std::shared_ptr<Image> image;
    TextureWrap texture_wrap { 1.0f, 1.0f };
    bool generate_mipmap { false };

    GLuint id { 0 };
    GLuint pbo[2] { 0, 0 };
    long frame { 0 };
    int width { 0 };
    int height { 0 };
    std::string name { "sampler" };
  };

} // namespace ZD
