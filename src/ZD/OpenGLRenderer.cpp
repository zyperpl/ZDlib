#include "OpenGLRenderer.hpp"
#include "Renderer.hpp"
#include "ShaderLoader.hpp"
#include "Window.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

#define OPENGL_ERROR_CALLBACK
#define GLFW_ERROR_CALLBACK

#define OPENGL_MAJOR 3
#define OPENGL_MINOR 3

static bool OGL_LOADED = false;

OGLRenderer::OGLRenderer()
{
  printf(
    "\nOGLRenderer initializing (OpenGL %d.%d)...\n",
    OPENGL_MAJOR,
    OPENGL_MINOR);
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  if ((OPENGL_MAJOR == 3 && OPENGL_MINOR >= 2) || OPENGL_MAJOR > 3)
  {
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  }

#ifdef GLFW_ERROR_CALLBACK
  glfwSetErrorCallback([](int error_code, const char* description) {
    fprintf(stderr, "GLFW ERROR %d: %s!\n", error_code, description);
    throw std::runtime_error("GLFW ERROR");
  });
#endif
}

OGLRenderer::~OGLRenderer()
{
  puts("Removing OpenGL renderer...\n");

  windows.clear();
  ShaderLoader::free_cache();

  uninitialize_gl();

  //glfwTerminate();
}

const Window* OGLRenderer::add_window(const WindowParameters& params)
{
  windows.push_back(std::make_unique<Window_GLFW>(params));
  assert(!windows.empty());
  puts("Window added.");

  initialize_gl();
  puts("GL initialized.");

  return windows.back().get();
}

void OGLRenderer::set_window_current(size_t index)
{
  Renderer::set_window_current(index);

  window()->set_current();
}

void OGLRenderer::remove_window(size_t index)
{
  window()->kill();

  Renderer::remove_window(index);
}

void OGLRenderer::initialize_gl()
{
  if (OGL_LOADED)
    return;

  // must be done after window creation
  assert(!windows.empty());

  puts("Initializing GL extension wrapper...");
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    printf("glewInit error\n");
    fprintf(stderr, "GLEW initialization failed!\n");
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Using GLEW %s\n", glewGetString(GLEW_VERSION));
#ifdef OPENGL_ERROR_CALLBACK

  auto OGLMessageCallback = [](
                              GLenum,
                              GLenum type,
                              GLuint,
                              GLenum severity,
                              GLsizei,
                              const GLchar* message,
                              const void*) {
    if (type == GL_DEBUG_TYPE_ERROR && severity != 0x9146)
    {
      static long glerrors = 0;
      fprintf(
        stderr,
        "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type,
        severity,
        message);

      glerrors++;
      if (glerrors >= 128)
        exit(3);
    }
  };

  if (glewGetExtension("GL_KHR_debug"))
  {
    puts("Enabling GL_DEBUG_OUTPUT");
    glEnable(GL_DEBUG_OUTPUT);
    puts("Setting glDebugMessageCallback");
    glDebugMessageCallback(OGLMessageCallback, 0);
  }
  else
  {
    puts("GL_KHR_debug not supported!");
  }

#endif

  glClearColor(0.9, 1.0, 0.9, 1.0);
  glfwSwapInterval(1);

  generate_vertex_array_object();
  OGL_LOADED = true;
}

void OGLRenderer::uninitialize_gl()
{
  if (!OGL_LOADED)
    return;

  puts("Deleting vertex array object...");
  OGL_LOADED = false;
  glDeleteVertexArrays(1, &vao);
}

void OGLRenderer::generate_vertex_array_object()
{
  puts("Generating vertex array object...");
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  printf("Vertex array object id=%u\n", vao);
}

void OGLRenderer::initialize_main_screen_image()
{
  puts("Initializing GL main screen image...");
  Renderer::initialize_main_screen_image();

  main_screen_texture = std::make_unique<Texture>(main_screen_image);

  current_shader_program =
    ShaderLoader()
      .add(ShaderDefault::ScreenTextureVertex, GL_VERTEX_SHADER)
      .add(ShaderDefault::ScreenTextureFragment, GL_FRAGMENT_SHADER)
      .compile();

  main_screen_model = std::make_unique<Model>(ModelDefault::Screen);
}

std::shared_ptr<Image> OGLRenderer::get_main_screen_image()
{
  if (!main_screen_image)
  {
    OGLRenderer::initialize_main_screen_image();
  }
  return main_screen_image;
}

void OGLRenderer::update() { glfwWaitEventsTimeout(1. / poll_rate); }

void OGLRenderer::clear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (main_screen_image)
  {
    main_screen_image->clear();
  }
}

void OGLRenderer::center_view_port()
{
  //TODO: execute only on window change
  //TODO: make should_center per window
  int width, height;
  glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

  const double desirable_width = window()->get_initial_width();
  const double desirable_height = window()->get_intial_height();

  double r = double(width) / desirable_width;
  const double r2 = double(height) / desirable_height;

  if (r > r2)
    r = r2;
  const int w = r * desirable_width;
  const int h = r * desirable_height;
  glViewport((width - w) / 2, (height - h) / 2, w, h);
}

void OGLRenderer::render_screen()
{
  if (screen_rendered)
    return;

  if (main_screen_texture)
  {
    main_screen_texture->update();
    main_screen_texture->bind(*current_shader_program.get());
  }

  if (current_shader_program)
  {
    current_shader_program->use();
  }

  if (main_screen_model)
  {
    main_screen_model->draw(*current_shader_program.get());
  }
  screen_rendered = true;
}

void OGLRenderer::render()
{
  if (!window()->is_open())
    return;

  if (should_center_view_port)
  {
    center_view_port();
  }

  render_screen();

  glfwSwapBuffers(glfwGetCurrentContext());

  screen_rendered = false;
}

void OGLRenderer::enable_blend(GLenum sfactor, GLenum dfactor)
{
  assert(!windows.empty());
  glEnable(GL_BLEND);
  glBlendFunc(sfactor, dfactor);
}

void OGLRenderer::disable_blend()
{
  assert(!windows.empty());
  glDisable(GL_BLEND);
}

void OGLRenderer::enable_cull_face(GLenum mode, GLenum front_face)
{
  assert(!windows.empty());
  glEnable(GL_CULL_FACE);
  glCullFace(mode);
  glFrontFace(front_face);
}

void OGLRenderer::disable_cull_face()
{
  assert(!windows.empty());
  glDisable(GL_CULL_FACE);
}

void OGLRenderer::enable_depth_test(GLenum func)
{
  assert(!windows.empty());
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(func);
}

void OGLRenderer::disable_depth_test()
{
  assert(!windows.empty());
  glDisable(GL_DEPTH_TEST);
}
