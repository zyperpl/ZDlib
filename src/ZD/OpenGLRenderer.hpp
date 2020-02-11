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

    bool should_center_view_port{true};
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
};
