#include "Shader.hpp"
#include <cassert>
#include <cstring>
#include <string>
#include <typeinfo>

#include "3rd/glm/glm.hpp"

ShaderProgram::ShaderProgram() { id = glCreateProgram(); }

ShaderProgram::~ShaderProgram() { glDeleteProgram(id); }

void ShaderProgram::extract_uniforms()
{
  assert(uniforms.empty());
  glUseProgram(id);

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

    GLint location = glGetUniformLocation(id, name);

    uniforms.insert(
      std::make_pair(std::string(name), ShaderUniform { id, i, size, type, location }));
    printf("%s: shader_id=%d id=%d size=%d type=%d location=%d\n", name, id, i, size, type, location);
  }
}

void ShaderProgram::extract_attributes()
{
  assert(attributes.empty());
  glUseProgram(id);

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

    GLint location = glGetAttribLocation(id, name);

    attributes.emplace(
      std::string(name), ShaderAttribute { id, i, size, type, location });
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

void ShaderProgram::use() const
{
  assert(is_linked());
  if (!is_linked())
  {
    fprintf(stderr, "ERROR: Shader program %d not linked!\n", id);
  }

  glUseProgram(id);
}

bool ShaderProgram::is_linked() const
{
  GLint link_status = GL_FALSE;
  glGetProgramiv(id, GL_LINK_STATUS, &link_status);
  return link_status == GL_TRUE;
}

template<typename T>
bool ShaderProgram::set_uniform(std::string name, T value)
{
  fprintf(
    stderr,
    "ShaderProgram::set_uniform %s cannot be set, unknown type %s!",
    name.data(),
    typeid(value).name());
  assert(false);
  return false;
}

template<>
bool ShaderProgram::set_uniform<glm::vec2>(std::string name, glm::vec2 value)
{
  if (auto uniform = get_uniform(name))
  {
    //printf("Setting %s to %f;%f\n", name.data(), value.x, value.y);
    glUniform2f(uniform->location, value.x, value.y);
    return true;
  }
  return false;
}

std::optional<ShaderUniform> ShaderProgram::get_uniform(std::string name) const
{
  if (uniforms.find(name) == uniforms.end())
  {
    return std::nullopt;
  }
  return uniforms.at(name);
}

std::optional<ShaderAttribute> ShaderProgram::get_attribute(
  std::string name) const
{
  if (attributes.find(name) == attributes.end())
  {
    return std::nullopt;
  }
  return attributes.at(name);
}
