#pragma once

#include <memory>
#include <map>
#include <string_view>
#include <variant>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ShaderLoader;
class ShaderProgram;

struct ShaderUniform
{
  GLuint program_id;
  GLint index;
  GLint size;
  GLenum type;
};

struct ShaderAttribute
{
  GLuint program_id;
  GLint index;
  GLint size;
  GLenum type;
};

class ShaderProgram
{
  public:
    ShaderProgram();
    ~ShaderProgram();
    std::optional<ShaderUniform> get_uniform(std::string name) const;
    std::optional<ShaderAttribute> get_attribute(std::string name) const;
    void use();
    void link();
    inline GLuint get_id() const { return id; }
  protected:

    void extract_uniforms();
    void extract_attributes();

  private:
    GLuint id;
    std::map<std::string, ShaderUniform> uniforms;
    std::map<std::string, ShaderAttribute> attributes;

    bool is_linked() const;

    friend class ShaderLoader;
};
