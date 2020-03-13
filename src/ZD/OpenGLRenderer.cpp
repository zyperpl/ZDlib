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

#ifdef OPENGL_ERROR_CALLBACK
static bool OGL_ERROR_CALLBACK_SET = false;

GLenum glCheckError_(const char* file, int line)
{
  GLenum errorCode;
  while ((errorCode = glGetError()) != GL_NO_ERROR)
  {
    std::string error;
    switch (errorCode)
    {
      case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
      case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
      case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
      case GL_STACK_OVERFLOW: error = "STACK_OVERFLOW"; break;
      case GL_STACK_UNDERFLOW: error = "STACK_UNDERFLOW"; break;
      case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        error = "INVALID_FRAMEBUFFER_OPERATION";
        break;
    }
    printf("OpenGL ERROR: %s | %s (%d)\n", error.data(), file, line);
  }
  return errorCode;
}
#else
GLenum glCheckError_(const char* file, int line) { return 0; }
#endif

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
  puts("Removing OpenGL renderer...");

  windows.clear();
  ShaderLoader::free_cache();

  uninitialize_gl();

  //glfwTerminate();
}

std::shared_ptr<Window> OGLRenderer::add_window(const WindowParameters& params)
{
  windows.push_back(std::make_shared<Window_GLFW>(params));

  initialize_gl();

  return windows.back();
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
  //fprintf(stdout, "Using GLEW %s\n", glewGetString(GLEW_VERSION));

  auto gl_str = glGetString(GL_VERSION);
  printf("OpenGL version available: %s.\n", gl_str);
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
      static int glerrors = 0;
      fprintf(
        stderr,
        "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type,
        severity,
        message);

      glerrors++;
      if (glerrors >= 500)
        exit(3);
    }
  };

  if (glewGetExtension("GL_KHR_debug"))
  {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(OGLMessageCallback, 0);
    OGL_ERROR_CALLBACK_SET = true;
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

  OGL_LOADED = false;
  glDeleteVertexArrays(1, &vao);
}

void OGLRenderer::generate_vertex_array_object()
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  //printf("Vertex array object id=%u\n", vao);
}

void OGLRenderer::update() { glfwWaitEventsTimeout(1. / poll_rate); }

void OGLRenderer::clear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OGLRenderer::center_view_port()
{
  //TODO: execute only on window change
  //TODO: make should_center per window
  int width, height;
  glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

  const double desirable_width = window()->get_view_width();
  const double desirable_height = window()->get_view_height();

  double r = double(width) / desirable_width;
  const double r2 = double(height) / desirable_height;

  if (r > r2)
    r = r2;
  const int w = r * desirable_width;
  const int h = r * desirable_height;
  glViewport((width - w) / 2, (height - h) / 2, w, h);
}

void OGLRenderer::render_screens()
{
  for (auto& screen : window()->get_screens())
  {
    //printf("rendering %p screen\n", &*screen);
    screen->render(*window());
  }
}

void OGLRenderer::render()
{
  if (!window()->is_open())
    return;

#ifdef OPENGL_ERROR_CALLBACK
  if (!OGL_ERROR_CALLBACK_SET)
  {
    static int gl_errors = 0;
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
      fprintf(stderr, "OpenGL ERROR: %d\n", err);
      gl_errors++;
    }
    if (gl_errors > 60)
    {
      exit(3);
    }
  }
#endif

  if (should_center_view_port)
  {
    center_view_port();
  }

  render_screens();

  glfwSwapBuffers(glfwGetCurrentContext());
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
