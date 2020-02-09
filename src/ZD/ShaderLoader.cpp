#include <cassert>
#include <memory>
#include <vector>

#include "ShaderLoader.hpp"

std::vector<ShaderInfo::Program> cached_programs;
std::vector<ShaderInfo::Shader> cached_shaders;

const GLchar* z_basicVertexSource = R"glsl(
  #ifdef GL_ES
    precision highp float;
  #endif

  attribute vec2 position;

  varying vec2 uv;

  void main()
  {
    gl_Position = vec4(position, 1.0, 1.0);
    uv = gl_Position.xy / 2.0 + 0.5;
    uv.y = 1.0-uv.y;
  }
)glsl";
const GLchar* z_basicFragmentSource = R"glsl(
  #ifdef GL_ES
    precision highp float;
  #endif
  varying vec2 uv;
  uniform sampler2D sampler;
  void main()
  {
    gl_FragColor = texture2D(sampler, uv);
  }
)glsl";

auto printErrors(void (*fInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*), GLuint sop)->int
{
  int logLength; 
  char *log = new char[512]; 
  fInfoLog(sop, 512, &logLength, log); 
  
  if (logLength > 0) 
  {
    printf("Compilation error:\n%s\n", log); 
  }
  
  delete[] log;
  return logLength;
};

std::optional<ShaderInfo::Shader> find_shader_in_cache(const std::string_view name, const GLuint type) 
{
  for (const ShaderInfo::Shader &shader : cached_shaders)
  {
    if (shader.type != type) continue;

    if (auto other_name = std::get_if<std::string_view>(&shader.name)) 
    {
      if (*other_name == name)
      {
        return shader;
      }
    }
  }
  return std::nullopt;
}

std::optional<ShaderInfo::Shader> find_shader_in_cache(const ShaderDefault name, const GLuint type) 
{
  for (const ShaderInfo::Shader &shader : cached_shaders)
  {
    if (shader.type != type) continue;

    if (auto other_name = std::get_if<ShaderDefault>(&shader.name)) 
    {
      if (*other_name == name)
      {
        return shader;
      }
    }
  }
  return std::nullopt;
}

std::optional<std::shared_ptr<ShaderProgram>> find_program_in_cache(const std::vector<ShaderInfo::Shader> shaders) 
{
  for (const ShaderInfo::Program &program_info : cached_programs)
  {
    bool the_same = true;
    for (const ShaderInfo::Shader &sh : shaders)
    {
      for (const ShaderInfo::Shader &other_sh : program_info.shaders)
      {
        if (other_sh.name != sh.name || other_sh.type != sh.type)
        {
          the_same = false;
        }
      }
    }
    if (the_same) {
      return program_info.program;
    }
  }
  return std::nullopt;
}


ShaderLoader &ShaderLoader::add(ShaderDefault name, GLuint type)
{
  assert(compiled_program == nullptr); 

  if (auto shader_info = find_shader_in_cache(name, type))
  {
    printf("Shader default_name=%d found in cache (%zu records).\n", (int)name, cached_shaders.size());
    loaded_shaders.push_back(*shader_info);
    return *this;
  }

  GLuint shader_id = ShaderLoader::load_shader(name, type);
  assert(shader_id > 0);

  ShaderInfo::Shader shader_info{shader_id, name, type};
  cached_shaders.push_back(shader_info);
  loaded_shaders.push_back(shader_info);

  return *this;
}

ShaderLoader &ShaderLoader::add(std::string_view name, GLuint type)
{
  assert(compiled_program == nullptr); 
  
  if (auto shader_info = find_shader_in_cache(name, type))
  {
    printf("Shader '%s' found in cache (%zu records).\n", name.data(), cached_shaders.size());
    loaded_shaders.push_back(shader_info.value());
    return *this;
  }

  GLuint shader_id = ShaderLoader::load_shader(name, type);
  assert(shader_id > 0);

  ShaderInfo::Shader shader_info{shader_id, name, type};
  cached_shaders.push_back(shader_info);
  loaded_shaders.push_back(shader_info);

  return *this;
}

std::shared_ptr<ShaderProgram> ShaderLoader::compile()
{
  if (compiled_program) {
    return compiled_program;
  }

  std::shared_ptr<ShaderProgram> program = std::make_shared<ShaderProgram>();
  auto s_id = program->get_id();

  for (const ShaderInfo::Shader &shader_info : loaded_shaders)
  {
    glAttachShader(s_id, shader_info.id);
  }
  program->link();
  printErrors(glGetProgramInfoLog, program->id);

  compiled_program = program;

  return compiled_program;
}

GLuint ShaderLoader::load_shader(ShaderDefault default_name, GLuint type)
{
  const char *shader_source = nullptr;

  switch (default_name)
  {
    case ShaderDefault::ScreenTextureVertex:
      shader_source = z_basicVertexSource;
      break;
    case ShaderDefault::ScreenTextureFragment:
      shader_source = z_basicFragmentSource;
      break;

    case ShaderDefault::Invalid:
    default:
      assert(false);
  }

  GLuint id = glCreateShader(type);
  glShaderSource(id, 1, &shader_source, NULL);
  glCompileShader(id);
  if (printErrors(glGetShaderInfoLog, id) > 0) return 0;

  return id;
}

GLuint ShaderLoader::load_shader(std::string_view , GLuint )
{
  assert(false);
  //TODO: add File class
  return 0;
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
