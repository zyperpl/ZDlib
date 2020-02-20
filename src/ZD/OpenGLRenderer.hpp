#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "Window.hpp"
#include "Size.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"

class OGLRenderer : public Renderer
{
  public:
    OGLRenderer();
    ~OGLRenderer();
    
    void set_window_current(size_t index);
    void remove_window(size_t index);
    const Window* add_window(const WindowParameters &params);

    std::shared_ptr<Image> get_main_screen_image();

    void clear();
    void update();
    void render();

    void render_screen();

    bool should_center_view_port{true};

    void enable_blend(
        GLenum sfactor = GL_SRC_ALPHA, 
        GLenum dfactor = GL_ONE_MINUS_SRC_ALPHA) 
    {
      assert(!windows.empty());
      glEnable(GL_BLEND);
      glBlendFunc(sfactor, dfactor);
    }
    void disable_blend() 
    {
      assert(!windows.empty());
      glDisable(GL_BLEND);
    }
    void enable_cull_face(
        GLenum mode = GL_FRONT, 
        GLenum front_face = GL_CW) 
    {
      assert(!windows.empty());
      glEnable(GL_CULL_FACE);
      glCullFace(mode); 
      glFrontFace(front_face);
    }
    void disable_cull_face() 
    {
      assert(!windows.empty());
      glDisable(GL_CULL_FACE);
    }
    void enable_depth_test(GLenum func = GL_LEQUAL)
    {
      assert(!windows.empty());
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(func);
    }
    void disable_depth_test()
    {
      assert(!windows.empty());
      glDisable(GL_DEPTH_TEST);
    }
  private:
    void generate_vertex_array_object();
    void initialize_gl();
    void uninitialize_gl();
    void initialize_main_screen_image();
    void center_view_port();
    GLuint vao;
    std::unique_ptr<Texture> main_screen_texture;
    std::unique_ptr<Model> main_screen_model;
    std::shared_ptr<ShaderProgram> current_shader_program;

    bool screen_rendered{false};
};
