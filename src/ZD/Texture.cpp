#include "Texture.hpp"
#include "Shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "OpenGLRenderer.hpp"

Texture::Texture(std::shared_ptr<Image> image)
: image { image }
{
  this->generate(TextureParameters {});
}

Texture::Texture(std::shared_ptr<Image> image, TextureParameters params)
: image { image }
, texture_wrap { params.wrap }
, generate_mipmap { params.generate_mipmap }
{
  this->generate(params);
}

Texture::~Texture()
{
  glDeleteTextures(1, &id);
  glDeleteBuffers(2, pbo);
}

void Texture::generate(TextureParameters params)
{
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA8,
    image->get_size().width(),
    image->get_size().height(),
    0,
    GL_BGRA,
    GL_UNSIGNED_INT_8_8_8_8,
    &image->get_data()[0]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.mag_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.min_filter);

  glBindTexture(GL_TEXTURE_2D, this->id);
  if (generate_mipmap)
  {
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  if (glewGetExtension("GL_ARB_pixel_buffer_object"))
  {
    glGenBuffers(2, pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[0]);
    glBufferData(
      GL_PIXEL_UNPACK_BUFFER,
      image->get_size().area() * sizeof(uint32_t),
      image->get_data(),
      GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[1]);
    glBufferData(
      GL_PIXEL_UNPACK_BUFFER,
      image->get_size().area() * sizeof(uint32_t),
      image->get_data(),
      GL_STREAM_DRAW);
    printf("Generated Pixel Buffer Object id 1=%u id 2=%u\n", pbo[0], pbo[1]);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  }

  glCheckError();
}

void Texture::update(Image *new_image)
{
  this->image.reset(new_image);
  this->update();
}

void Texture::update()
{
  if (!image)
    return;

  auto *data_ptr = &image->get_data()[0];

  if (pbo[frame % 2] > 0)
  {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[frame % 2]);
    data_ptr = 0;
  }
  
  glBindTexture(GL_TEXTURE_2D, this->id);
  glTexSubImage2D(
    GL_TEXTURE_2D,
    0,
    0,
    0,
    image->get_size().width(),
    image->get_size().height(),
    GL_BGRA,
    GL_UNSIGNED_INT_8_8_8_8,
    data_ptr);

  if (generate_mipmap)
  {
    glBindTexture(GL_TEXTURE_2D, this->id);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  
  if (pbo[(frame+1) % 2] > 0)
  {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[(frame+1) % 2]);
    auto *pbo_ptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    if (pbo_ptr)
    {
      memcpy(pbo_ptr, &image->get_data()[0], image->get_size().area() * sizeof(uint32_t));
      glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    frame++;
  }
}

void Texture::bind(const ShaderProgram &shader)
{
  glActiveTexture(GL_TEXTURE0 + this->sampler_id);
  glBindTexture(GL_TEXTURE_2D, this->id);

  if (auto sampler_uniform = shader.get_uniform("sampler"))
  {
    assert(sampler_uniform->type == GL_SAMPLER_2D);
    glUniform1i(sampler_uniform->location, this->sampler_id);
  }

  if (auto wrap_uniform = shader.get_uniform("texture_wrap"))
  {
    assert(wrap_uniform->type == GL_FLOAT_VEC2);
    glUniform2f(
      wrap_uniform->location, this->texture_wrap.x, this->texture_wrap.y);
  }

  glCheckError();

  if (frame % 2 == 1)
  {
    update();
  }
}
