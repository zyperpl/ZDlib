#pragma once

#include <initializer_list>
#include <memory>
#include <vector>

#include "Shader.hpp"
#include "File.hpp"

enum class ShaderDefault
{
  ScreenTextureVertex,
  ScreenTextureFragment,
  CenterModelTextureVertex,
  CenterModelTextureFragment,
  Invalid
};

namespace ShaderInfo
{
  struct Shader
  {
    GLuint id;
    /* Can be name as string_view, data as string_view or type as ShaderDefault enum */
    std::variant<std::string_view, ShaderDefault> unique_identifier;
    GLuint type;
  };

  struct Program
  {
    std::shared_ptr<ShaderProgram> program;
    std::vector<Shader> shaders;
  };
} // namespace ShaderInfo

class ShaderLoader
{
public:
  ShaderLoader() = default;

  ShaderLoader &add(ShaderDefault name, GLuint type);
  ShaderLoader &add(const File &file, GLuint type);
  ShaderLoader &add(const std::string_view data, GLuint type);
  std::shared_ptr<ShaderProgram> compile();

  static void free_cache();

private:
  std::vector<ShaderInfo::Shader> loaded_shaders;
  std::shared_ptr<ShaderProgram> compiled_program { nullptr };

  static GLuint load_shader(ShaderDefault default_name, GLuint type);
  static GLuint load_shader(std::string_view data, GLuint type);

  friend class Shader;
};
