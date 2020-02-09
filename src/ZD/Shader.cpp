#include "Shader.hpp"
#include <cassert>

ShaderProgram::ShaderProgram()
{
  id = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
  glDeleteProgram(id);
}

void ShaderProgram::extract_uniforms()
{
  assert(uniforms.empty());

  GLint count = 0;
  glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);

  GLint max_length = 0;
  glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);

  for (GLint i = 0; i < count; i++)
  {
    GLchar name[max_length];

    GLint size;
    GLenum type;
    GLsizei length;
    glGetActiveUniform(id, (GLuint)i, max_length, &length, &size, &type, name);
    name[length] = '\0';

    uniforms.emplace(std::string(name), ShaderUniform{ id, i, size, type });
  }
}

void ShaderProgram::extract_attributes()
{
  assert(attributes.empty());

  GLint count = 0;
  glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &count);

  GLint max_length = 0;
  glGetProgramiv(id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);

  for (GLint i = 0; i < count; i++)
  {
    GLchar name[max_length];

    GLint size;
    GLenum type;
    GLsizei length;
    glGetActiveAttrib(id, (GLuint)i, max_length, &length, &size, &type, name);
    name[length] = '\0';

    attributes.emplace(std::string(name), ShaderAttribute{ id, i, size, type });
  }
}

void ShaderProgram::link()
{
  assert(!is_linked());

  glLinkProgram(id);

  extract_uniforms();
  extract_attributes();

  assert(is_linked());
}

void ShaderProgram::use() 
{ 
  assert(is_linked());

  glUseProgram(id); 
}

bool ShaderProgram::is_linked() const
{
  GLint link_status = GL_FALSE;
  glGetProgramiv(id, GL_LINK_STATUS, &link_status);
  return link_status == GL_TRUE;
}

std::optional<ShaderUniform> ShaderProgram::get_uniform(std::string name) const
{
  if (uniforms.find(name) == uniforms.end()) {
    return std::nullopt;
  }
  return uniforms.at(name);
}

std::optional<ShaderAttribute> ShaderProgram::get_attribute(std::string name) const
{
  if (attributes.find(name) == attributes.end()) {
    return std::nullopt;
  }
  return attributes.at(name);
}
