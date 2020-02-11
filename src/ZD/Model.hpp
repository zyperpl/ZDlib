#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string_view>
#include <vector>

#include "Shader.hpp"

enum class ModelDefault
{
  Screen, Cube, Ball, Plane, Invalid
};

class Model
{
  public:
    Model(ModelDefault default_name);
    Model(std::string_view file_name);
    ~Model();
    void generate_vbo();
    void generate_ebo();
    void draw(const ShaderProgram &program);
  private:
    GLuint vbo{0};
    GLuint ebo{0};
    GLuint uvbo{0};
    std::vector<GLfloat> vertices;
    std::vector<GLuint> elements;
    std::vector<GLfloat> uvs;
};