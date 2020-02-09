#pragma once

#include <initializer_list>
#include <memory>
#include <vector>

#include "Shader.hpp"

enum class ShaderDefault
{
  ScreenTextureVertex, ScreenTextureFragment, Invalid
};

namespace ShaderInfo
{
  struct Shader
  {
    GLuint id;
    std::variant<std::string_view, ShaderDefault> name;
    GLuint type;
  };

  struct Program
  {
    std::shared_ptr<ShaderProgram> program;
    std::vector<Shader> shaders;
  };
}

class ShaderLoader
{
public:
  ShaderLoader() = default;

  ShaderLoader &add(ShaderDefault name, GLuint type);
  ShaderLoader &add(std::string_view name, GLuint type);
  std::shared_ptr<ShaderProgram> compile();

  static void free_cache();
private:
  std::vector<ShaderInfo::Shader> loaded_shaders;
  std::shared_ptr<ShaderProgram> compiled_program{nullptr};

  static GLuint load_shader(ShaderDefault default_name, GLuint type);
  static GLuint load_shader(std::string_view name, GLuint type);

  friend class Shader;
};
