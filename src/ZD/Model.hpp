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
    Model() {}
    Model(ModelDefault default_name);
    Model(std::string_view file_name);
    ~Model();
    void regenerate_buffers();
    void generate_vbo();
    void generate_ebo();
    void draw(const ShaderProgram &program);

    void add_vertex(const GLfloat v) { vertices.push_back(v); }
    void add_vertex(const GLfloat v1, const GLfloat v2, const GLfloat v3) { vertices.insert(vertices.end(), { v1, v2, v3 }); }

    void add_uv(const GLfloat u_or_v) { uvs.push_back(u_or_v); }
    void add_uv(const GLfloat u, const GLfloat v) { uvs.push_back(u); uvs.push_back(v); }

    void add_normal(const GLfloat n) { normals.push_back(n); }
    void add_normal(const GLfloat nv1, const GLfloat nv2, const GLfloat nv3) { normals.insert(normals.end(), { nv1, nv2, nv3 }); }

    const std::vector<GLfloat> &get_vertices() const { return vertices; }
    const std::vector<GLuint> &get_elements() const { return elements; }
    const std::vector<GLfloat> &get_uvs() const { return uvs; }
    const std::vector<GLfloat> &get_normals() const { return normals; }

  private:
    GLuint vbo { 0 };
    GLuint ebo { 0 };
    GLuint uvbo { 0 };
    GLuint nbo { 0 };
    std::vector<GLfloat> vertices;
    std::vector<GLuint> elements;
    std::vector<GLfloat> uvs;
    std::vector<GLfloat> normals;
  };
} // namespace ZD
