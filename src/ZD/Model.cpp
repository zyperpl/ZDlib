#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <memory>
#include <vector>

#include "Model.hpp"

const std::vector<GLfloat> z_screen_vertices { 
  1.0, -1.0, 
  -1.0, -1.0, 
  -1.0, 1.0, 
  1.0, 1.0
};

const int z_screen_components_per_vertex = 2;

const std::vector<GLuint> z_screen_elements {
  0, 1, 2, 0, 2, 3
};

Model::Model(ModelDefault default_name)
{
  switch (default_name)
  {
    case ModelDefault::Screen:
      vertices = z_screen_vertices;
      elements = z_screen_elements;
      components_per_vertex = z_screen_components_per_vertex;
      break;
    case ModelDefault::Cube:
    case ModelDefault::Ball:
    case ModelDefault::Plane:
    default:
      assert(false);
  }

  this->generate_vbo();

  if (!elements.empty()) {
    this->generate_ebo();
  }
}

Model::Model(std::string_view file_name)
{
  assert(false);
  assert(!file_name.empty());
  //TODO: add file class
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
  assert(!vertices.empty());

  auto position_attribute = program.get_attribute("position");
  if (!position_attribute) return;

  const GLuint pos_attr_index = position_attribute->index;

  glEnableVertexAttribArray(pos_attr_index);
  glVertexAttribPointer(pos_attr_index, components_per_vertex, GL_FLOAT, GL_FALSE, 0, (void*)0);

  if (ebo > 0)
  {
    assert(!elements.empty());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, NULL);
  } else
  {
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
  }

  glDisableVertexAttribArray(pos_attr_index);
}
