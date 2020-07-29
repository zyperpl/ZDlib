#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string_view>
#include <vector>

#include "Texture.hpp"
#include "Shader.hpp"
namespace ZD
{
  enum class ModelDefault
  {
    Screen,
    Cube,
    Ball,
    Plane,
    Invalid
  };

  class Model
  {
  public:
    Model(ModelDefault default_name);
    Model(std::string_view file_name);
    ~Model();
    void generate_vbo();
    void generate_ebo();
    inline void add_texture(std::shared_ptr<Texture> texture)
    {
      textures.push_back(texture);
    }
    void draw(const ShaderProgram &program);

  private:
    GLuint vbo { 0 };
    GLuint ebo { 0 };
    GLuint uvbo { 0 };
    GLuint nbo { 0 };
    std::vector<GLfloat> vertices;
    std::vector<GLuint> elements;
    std::vector<GLfloat> uvs;
    std::vector<GLfloat> normals;

    std::vector<std::shared_ptr<Texture>> textures;
  };
} // namespace ZD
