#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <memory>
#include <vector>

#include "Model.hpp"

#pragma GCC optimize ("O3")

const std::vector<GLfloat> z_screen_vertices { 
  1.0, -1.0, 
  -1.0, -1.0, 
  -1.0, 1.0, 
  1.0, 1.0
};

const std::vector<GLuint> z_screen_elements {
  0, 1, 2, 0, 2, 3
};

const std::vector<GLfloat> z_cube_vertices {
  -1.,-1.,-1., -1.,-1., 1., -1., 1., 1., 
  1., 1.,-1., -1.,-1.,-1., -1., 1.,-1.,
  1.,-1., 1., -1.,-1.,-1., 1.,-1.,-1.,
  1., 1.,-1., 1.,-1.,-1., -1.,-1.,-1.,
  -1.,-1.,-1., -1., 1., 1., -1., 1.,-1.,
  1.,-1., 1., -1.,-1., 1., -1.,-1.,-1.,
  -1., 1., 1., -1.,-1., 1., 1.,-1., 1.,
  1., 1., 1., 1.,-1.,-1., 1., 1.,-1.,
  1.,-1.,-1., 1., 1., 1., 1.,-1., 1.,
  1., 1., 1., 1., 1.,-1., -1., 1.,-1.,
  1., 1., 1., -1., 1.,-1., -1., 1., 1.,
  1., 1., 1., -1., 1., 1., 1.,-1., 1.
};

const std::vector<GLfloat> z_cube_uvs {
  0., 0., 1., 0., 1., 1.,  0., 1., 1., 1., 0., 0.,
  0., 1., 1., 0., 0., 0.,  0., 0., 0., 1., 1., 1.,
  0., 0., 1., 0., 1., 1.,  0., 0., 0., 1., 1., 1.,
  0., 0., 1., 0., 1., 1.,  0., 0., 0., 1., 1., 1.,
  0., 0., 1., 0., 1., 1.,  0., 0., 0., 1., 1., 1.,
  0., 0., 1., 0., 1., 1.,  0., 0., 0., 1., 1., 1.,
};

template <typename T>
int generate_gl_buffer(std::vector<T> data)
{
  GLuint id = 0;

  glGenBuffers(1, &id);

  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, 
               data.size() * sizeof(T), 
               data.data(), 
               GL_STATIC_DRAW);
  printf("generated buffer id=%u\n", id);

  return id;
}

Model::Model(ModelDefault default_name)
{
  switch (default_name)
  {
    case ModelDefault::Screen:
      vertices = z_screen_vertices;
      elements = z_screen_elements;
      break;

    case ModelDefault::Cube:
      vertices = z_cube_vertices;
      uvs = z_cube_uvs;
      break;

    case ModelDefault::Ball:
    case ModelDefault::Plane:
    default:
      assert(false);
  }

  this->generate_vbo();

  if (!elements.empty()) {
    this->generate_ebo();
  }

  if (!uvs.empty()) {
    uvbo = generate_gl_buffer(uvs);
  }
}

Model::Model(std::string_view file_name)
{
  assert(false);
  assert(!file_name.empty());
  //TODO: add file class
}

Model::~Model()
{
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glDeleteBuffers(1, &uvbo);
}

void Model::generate_vbo()
{
  assert(!vertices.empty());

  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 
               vertices.size() * sizeof(GLfloat), 
               vertices.data(), 
               GL_STATIC_DRAW);

  assert(vbo > 0);
}

void Model::generate_ebo()
{
  glGenBuffers(1, &ebo);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
               elements.size() * sizeof(GLint), 
               elements.data(), 
               GL_STATIC_DRAW);

  assert(ebo > 0);
}

void Model::draw(const ShaderProgram &program)
{
  auto position_attribute = program.get_attribute("position");
  assert(position_attribute);

  int components_per_vertex = 3;
  switch (position_attribute->type)
  {
    case GL_FLOAT: components_per_vertex = 1;  break;
    case GL_FLOAT_VEC2: components_per_vertex = 2;  break;
    case GL_FLOAT_VEC3: components_per_vertex = 3;  break;
    case GL_FLOAT_VEC4: components_per_vertex = 4;  break;
  }

  const GLuint pos_attr_index = position_attribute->index;

  glEnableVertexAttribArray(pos_attr_index);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(pos_attr_index, components_per_vertex, GL_FLOAT, GL_FALSE, 0, (void*)0);

  if (uvbo > 0)
  {
    auto uv_attribute = program.get_attribute("vertex_uv");
    assert(uv_attribute);

    glEnableVertexAttribArray(uv_attribute->index);
    glBindBuffer(GL_ARRAY_BUFFER, uvbo);
    glVertexAttribPointer(uv_attribute->index, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  }

  if (ebo > 0)
  {
    assert(!elements.empty());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, NULL);
  } else
  {
    assert(!vertices.empty());
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
  }

  glDisableVertexAttribArray(pos_attr_index);
}
