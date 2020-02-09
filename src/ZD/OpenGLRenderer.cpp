#include "OpenGLRenderer.hpp"
#include "Renderer.hpp"
#include "ShaderLoader.hpp"

#define OPENGL_ERROR_CALLBACK

static bool OGL_LOADED = false;

OGLRenderer::OGLRenderer()
{
  printf("\nOGLRenderer initializing...\n");

  glfwInit();
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  
}

OGLRenderer::~OGLRenderer()
{
  ShaderLoader::free_cache();
}

size_t OGLRenderer::add_window(const WindowParameters &params)
{
  windows.push_back(std::make_shared<Window_GLFW>(params)); 

  if (!OGL_LOADED)
  {
    // must be done after window creation
    initialize_gl();
    generate_vertex_array_object();
    OGL_LOADED = true;
  }

  return windows.size() - 1;
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
  glewExperimental = GL_TRUE; 
  glewInit();
  glGetError(); //reset error from glewInit

  #ifdef OPENGL_ERROR_CALLBACK

  auto OGLMessageCallback = [](GLenum, GLenum type, GLuint, GLenum severity, 
                                  GLsizei, const GLchar* message, const void*)
  {
    if (type == GL_DEBUG_TYPE_ERROR && severity != 0x9146)
    {
      static long glerrors = 0;
      fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
              (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                type, severity, message);

      glerrors++;
      if (glerrors >= 128) exit(3);
    }
  };

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(OGLMessageCallback, 0);

  #endif
}

void OGLRenderer::generate_vertex_array_object()
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
}

void OGLRenderer::initialize_main_screen_image()
{
  Renderer::initialize_main_screen_image();

  main_screen_texture = std::make_unique<Texture>(main_screen_image);

  current_shader_program = 
    ShaderLoader()
    .add(ShaderDefault::ScreenTextureVertex, GL_VERTEX_SHADER)
    .add(ShaderDefault::ScreenTextureFragment, GL_FRAGMENT_SHADER)
    .compile();

  main_screen_model = std::make_unique<Model>(ModelDefault::Screen);
}

std::shared_ptr<Image> OGLRenderer::get_main_screen_image() {
  if (!main_screen_image) {
    OGLRenderer::initialize_main_screen_image();
  }
  return main_screen_image;
}

void OGLRenderer::update()
{
  glfwPollEvents();
  glfwSwapBuffers(glfwGetCurrentContext());
}

void OGLRenderer::clear()
{
  if (main_screen_image) {
    main_screen_image->clear();
  }
}

void OGLRenderer::render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (main_screen_texture)
  {
    main_screen_texture->update();
    main_screen_texture->bind(*current_shader_program.get());

    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

    double r  = double(width)  / double(main_screen_image->get_size().width());
    double r2 = double(height) / double(main_screen_image->get_size().height());

    if (r > r2) r = r2;
    int w = int(r*main_screen_image->get_size().width());
    int h = int(r*main_screen_image->get_size().height());
    glViewport((width-w)/2, (height-h)/2, w, h);

    //main_screen_image->print();
  }

  if (current_shader_program)
  {
    current_shader_program->use();
  }

  if (main_screen_model)
  {
    main_screen_model->draw(*current_shader_program.get());
  }
}
