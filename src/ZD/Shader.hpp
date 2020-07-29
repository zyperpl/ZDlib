#pragma once

#include <memory>
#include <unordered_map>
#include <string_view>
#include <variant>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
namespace ZD
{
  class ShaderLoader;
  class ShaderProgram;

  struct ShaderUniform
  {
    GLuint program_id;
    GLint index;
    GLint size;
    GLenum type;
    GLint location;
  };

  struct ShaderAttribute
  {
    GLuint program_id;
    GLint index;
    GLint size;
    GLenum type;
    GLint location;
  };

  class ShaderProgram
  {
  public:
    ShaderProgram();
    ~ShaderProgram();

    template<typename T>
    bool set_uniform(std::string name, T value);

    std::optional<ShaderUniform> get_uniform(std::string name) const;
    std::optional<ShaderAttribute> get_attribute(std::string name) const;
    void use() const;

    inline GLuint get_id() const { return id; }

  protected:
    void extract_uniforms();
    void extract_attributes();

  private:
    GLuint id;
    std::unordered_map<std::string, ShaderUniform> uniforms;
    std::unordered_map<std::string, ShaderAttribute> attributes;

    void link();
    bool is_linked() const;

    friend class ShaderLoader;
  };

} // namespace ZD
