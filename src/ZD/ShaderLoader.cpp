#include <cassert>
#include <memory>
#include <vector>

#include "ShaderLoader.hpp"

#pragma GCC optimize("O3")

std::vector<ShaderInfo::Program> cached_programs;
std::vector<ShaderInfo::Shader> cached_shaders;

const GLchar *z_screen_texture_vertex_shader = R"glsl(
  #version 330 
  precision highp float;

  in vec2 position;
  out vec2 uv;
  uniform vec2 framebuffer_size;
  uniform vec2 view_size;
  uniform vec2 screen_position;
  uniform vec2 screen_scale;
  uniform vec2 texture_size;
  
  void main()
  {
    vec2 pixel_size = view_size / texture_size / 2.;
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    uv = gl_Position.xy / 2.0 + 0.5;
    uv.y = 1.0 - uv.y;
    uv += pixel_size * 0.000005;
    gl_Position.xy *= screen_scale;
    gl_Position.xy += (vec2(screen_position.x, -screen_position.y) / view_size) * 2.0;
  }
)glsl";
const GLchar *z_texture_frag_shader = R"glsl(
  #version 330 

  in vec2 uv;
  uniform sampler2D sampler;

  out vec4 fragColor;
  void main()
  {
    fragColor = texture(sampler, uv);
  }
)glsl";

const GLchar *z_mvp_model_vertex_shader = R"glsl(
  #version 330 

  #ifdef GL_ES
    precision highp float;
  #endif

  uniform mat4 M; 
  uniform mat4 V; 
  uniform mat4 P; 

  in vec3 position;
  in vec2 vertex_uv;
  in vec3 vertex_normal;

  out vec2 uv;

  void main()
  {
    gl_Position = P * V * M * vec4(position, 1.0);
    uv = vertex_uv;
  }
)glsl";

const GLchar *z_tileset_vertex_shader = R"glsl(
  #version 330 
  precision highp float;

  in vec2 position;
  out vec2 map_uv;
  out vec2 screen_uv;
  uniform vec2 framebuffer_size;
  uniform vec2 view_size;
  uniform vec2 view_scale;
  uniform vec2 view_offset;
  uniform vec2 screen_position;
  uniform vec2 screen_scale;
  uniform vec2 texture_size;
  uniform vec2 tile_size;
  
  void main()
  {
    vec2 pixel_size = view_size / texture_size;
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    vec2 uv = gl_Position.xy / 2.0 + 0.5;
    uv.y = 1.0 - uv.y;
    screen_uv = uv * view_size / view_scale + view_offset;
    screen_uv += pixel_size * 0.02;
    map_uv = screen_uv / texture_size / tile_size;
    gl_Position.xy *= screen_scale;
    gl_Position.xy += (vec2(screen_position.x, -screen_position.y) / view_size) * 2.;
  }
)glsl";
const GLchar *z_tileset_fragment_shader = R"glsl(
  #version 330 

  in vec2 map_uv;
  in vec2 screen_uv;
  uniform sampler2D tileset_sampler;
  uniform sampler2D map_sampler;
  uniform vec2 spritesheet_size;
  uniform vec2 tile_size;

  out vec4 fragColor;
  void main()
  {
    if (screen_uv.x < 0.0 || screen_uv.y < 0.0) { discard; }
    vec2 uv = texture(map_sampler, map_uv).rg;
    if (uv.x == 1.0 && uv.y == 1.0) { discard; }
    vec2 spriteOffset = floor(uv * 256.0) * tile_size;
    vec2 spriteCoord = mod(screen_uv, tile_size);
    fragColor = texture(tileset_sampler, (spriteOffset + spriteCoord) / spritesheet_size);
  }
)glsl";

auto print_shader_errors(
  void (*fInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *), GLuint sop) -> int
{
  int logLength;
  char *log = new char[512];
  fInfoLog(sop, 512, &logLength, log);

  if (logLength > 0) { printf("Compilation error:\n%s\n", log); }

  delete[] log;
  return logLength;
};

std::optional<ShaderInfo::Shader> find_shader_in_cache(
  const std::string_view name_or_data, const GLuint type)
{
  for (const ShaderInfo::Shader &shader : cached_shaders)
  {
    if (shader.type != type) continue;

    if (auto other_name = std::get_if<std::string_view>(&shader.unique_identifier))
    {
      if (*other_name == name_or_data) { return shader; }
    }
  }
  return std::nullopt;
}

std::optional<ShaderInfo::Shader> find_shader_in_cache(
  const ShaderDefault name, const GLuint type)
{
  for (const ShaderInfo::Shader &shader : cached_shaders)
  {
    if (shader.type != type) continue;

    if (auto other_name = std::get_if<ShaderDefault>(&shader.unique_identifier))
    {
      if (*other_name == name) { return shader; }
    }
  }
  return std::nullopt;
}

std::optional<std::shared_ptr<ShaderProgram>> find_program_in_cache(
  const std::vector<ShaderInfo::Shader> shaders)
{
  for (const ShaderInfo::Program &program_info : cached_programs)
  {
    bool the_same = true;
    for (const ShaderInfo::Shader &sh : shaders)
    {
      for (const ShaderInfo::Shader &other_sh : program_info.shaders)
      {
        if (other_sh.unique_identifier != sh.unique_identifier || other_sh.type != sh.type)
        { the_same = false; }
      }
    }
    if (the_same) { return program_info.program; }
  }
  return std::nullopt;
}

ShaderLoader &ShaderLoader::add(ShaderDefault name, GLuint type)
{
  assert(compiled_program == nullptr);

  if (auto shader_info = find_shader_in_cache(name, type))
  {
    //printf("Shader default_name=%d found in cache (%zu records).\n", (int)name, cached_shaders.size());
    loaded_shaders.push_back(*shader_info);
    return *this;
  }

  GLuint shader_id = ShaderLoader::load_shader(name, type);
  assert(shader_id > 0);

  ShaderInfo::Shader shader_info { shader_id, name, type };
  cached_shaders.push_back(shader_info);
  loaded_shaders.push_back(shader_info);

  return *this;
}

ShaderLoader &ShaderLoader::add(const File &file, GLuint type)
{
  assert(compiled_program == nullptr);

  if (auto shader_info = find_shader_in_cache(file.get_name(), type))
  {
    //printf("Shader '%s' found in cache (%zu records).\n", name.data(), cached_shaders.size());
    loaded_shaders.push_back(*shader_info);
    return *this;
  }

  GLuint shader_id =
    ShaderLoader::load_shader(std::string_view(file.read_all_chars()), type);
  assert(shader_id > 0);

  ShaderInfo::Shader shader_info { shader_id, file.get_name(), type };
  cached_shaders.push_back(shader_info);
  loaded_shaders.push_back(shader_info);

  return *this;
}

ShaderLoader &ShaderLoader::add(const std::string_view data, GLuint type)
{
  assert(compiled_program == nullptr);

  if (auto shader_info = find_shader_in_cache(data, type))
  {
    //printf("Shader '%s' found in cache (%zu records).\n", name.data(), cached_shaders.size());
    loaded_shaders.push_back(*shader_info);
    return *this;
  }

  GLuint shader_id =
    ShaderLoader::load_shader(data, type);
  assert(shader_id > 0);

  ShaderInfo::Shader shader_info { shader_id, data, type };
  cached_shaders.push_back(shader_info);
  loaded_shaders.push_back(shader_info);

  return *this;
}

std::shared_ptr<ShaderProgram> ShaderLoader::compile()
{
  if (compiled_program) { return compiled_program; }

  std::shared_ptr<ShaderProgram> program = std::make_shared<ShaderProgram>();
  auto s_id = program->get_id();

  for (const ShaderInfo::Shader &shader_info : loaded_shaders)
  {
    glAttachShader(s_id, shader_info.id);
  }
  program->link();
  print_shader_errors(glGetProgramInfoLog, program->id);
  assert(program->is_linked());

  compiled_program = program;

  return compiled_program;
}

GLuint ShaderLoader::load_shader(ShaderDefault default_name, GLuint type)
{
  const char *shader_source = nullptr;

  switch (default_name)
  {
    case ShaderDefault::ScreenTextureVertex:
      shader_source = z_screen_texture_vertex_shader;
      break;
    case ShaderDefault::ScreenTextureFragment:
    case ShaderDefault::CenterModelTextureFragment:
      shader_source = z_texture_frag_shader;
      break;
    case ShaderDefault::CenterModelTextureVertex:
      shader_source = z_mvp_model_vertex_shader;
      break;
    case ShaderDefault::TilesetVertex:
      shader_source = z_tileset_vertex_shader;
      break;
    case ShaderDefault::TilesetFragment:
      shader_source = z_tileset_fragment_shader;
      break;

    case ShaderDefault::Invalid:
    default: assert(false);
  }

  GLuint id = glCreateShader(type);
  glShaderSource(id, 1, &shader_source, NULL);
  glCompileShader(id);
  if (print_shader_errors(glGetShaderInfoLog, id) > 0) return 0;

  return id;
}

GLuint ShaderLoader::load_shader(std::string_view data, GLuint type)
{
  GLuint id = glCreateShader(type);
  const GLchar *shader_source = &data[0];
  glShaderSource(id, 1, &shader_source, NULL);
  glCompileShader(id);
  if (print_shader_errors(glGetShaderInfoLog, id) > 0) return 0;
  
  return id;
}

void ShaderLoader::free_cache()
{
  for (const auto &shader_info : cached_shaders)
  {
    glDeleteShader(shader_info.id);
  }

  cached_shaders.clear();
  cached_programs.clear();
}
