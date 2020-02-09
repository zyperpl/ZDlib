#include "Texture.hpp"
#include "Shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

Texture::Texture(std::shared_ptr<Image> image)
  : image{image}
{
  this->generate();
}

Texture::Texture(std::shared_ptr<Image> image, TextureParameters params)
  : image{image}
  , texture_wrap{params.wrap}
{
  this->generate();
}

void Texture::update(Image *new_image)
{
  this->image.reset(new_image);
  this->update();
}

void Texture::update()
{
  if (!image) return;

  GLenum format = GL_RGBA;
  if (image->get_format() == PixelFormat::Type::RGB) {
    format = GL_RGB;
  }

  glTexSubImage2D(GL_TEXTURE_2D, 
      0, 
      0, 
      0, 
      image->get_size().width(), 
      image->get_size().height(), 
      format, 
      GL_UNSIGNED_INT_8_8_8_8, 
      &image->get_data()[0]);
}

void Texture::generate()
{
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  GLenum format = GL_RGBA;
  GLint internal_format = GL_RGBA32UI;
  if (image->get_format() == PixelFormat::Type::RGB) {
    format = GL_RGB;
    internal_format = GL_RGB32UI;
  }

  printf("Texture %p format=%s\n", this, format == GL_RGB ? "RGB" : "RGBA");
  glTexImage2D(GL_TEXTURE_2D, 
      0, 
      format, 
      image->get_size().width(), 
      image->get_size().height(), 
      0, 
      format, 
      GL_UNSIGNED_INT_8_8_8_8, 
      &image->get_data()[0]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::bind(const ShaderProgram &shader)
{
  glActiveTexture(GL_TEXTURE0 + this->sampler_id);
  glBindTexture(GL_TEXTURE_2D, this->id);

  if (auto sampler_uniform = shader.get_uniform("sampler"))
  {
    assert(sampler_uniform->type == GL_SAMPLER_2D);
    glUniform1i(sampler_uniform->index, this->sampler_id);
  }

  if (auto wrap_uniform = shader.get_uniform("textureWrap")) 
  {
    assert(wrap_uniform->type == GL_FLOAT_VEC2);
    glUniform2f(wrap_uniform->index, this->texture_wrap.x, this->texture_wrap.y);
  } 
}
