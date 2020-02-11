#include "ZD/OpenGLRenderer.hpp"
#include "ZD/Model.hpp"
#include "ZD/Texture.hpp"
#include "ZD/Shader.hpp"
#include "ZD/ShaderLoader.hpp"
#include "ZD/Entity.hpp"
#include "ZD/View.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define W 1920/3
#define H 1080/3

int model_test_main(int, char**)
{
  auto renderer = OGLRenderer();
  renderer.add_window(WindowParameters(Size(W,H), "ZD model test") );

  auto cube = Model(ModelDefault::Cube);
  auto model_shader = ShaderLoader()
      .add(ShaderDefault::CenterModelTextureVertex, GL_VERTEX_SHADER)
      .add(ShaderDefault::CenterModelTextureFragment, GL_FRAGMENT_SHADER)
      .compile();

  auto lena_texture = Texture(Image::load("images/lena.png"));

  auto entity = Entity({0,0,0}, {0,0,0}, {.4,.4,.4});
  entity.add_model(cube);

  auto view = View(
      Camera::PerspectiveParameters(
        Camera::Fov(glm::radians(90.0f)), 
        float(W)/float(H),
        Camera::ClippingPlane(0.1f, 1000.f)
      ),
      { -1.0, 0.0, -1.0 }
    );

  glm::vec3 camera_position(-1,0,-1);

  while (renderer.is_window_open())
  {
    renderer.update();
    renderer.clear();

    float step = 1.0;
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT)) step *= 10.;
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_CONTROL)) step *= 10.;

    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A)) {
      //entity.move_position({-.0001*step, 0, 0});
      camera_position.x -= .0001*step;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D)) {
      //entity.move_position({.0001*step, 0, 0});
      camera_position.x += .0001*step;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W)) {
      //entity.move_position({0, .0001*step, 0});
      camera_position.y += .0001*step;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S)) {
      //entity.move_position({0, -.0001*step, 0});
      camera_position.y -= .0001*step;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_E)) {
      //entity.move_position({0, 0, .0001*step});
      camera_position.z += .0001*step;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_Q)) {
      //entity.move_position({0, 0, -.0001*step});
      camera_position.z -= .0001*step;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_R)) {
      entity.add_rotation({0.001*step, 0, 0});
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_F)) {
      entity.add_rotation({0, 0.001*step, 0});
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_V)) {
      entity.add_rotation({0, 0, 0.001*step});
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_C)) {
      entity.multiply_scale(1.00 + 0.001*step);
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_Z)) {
      entity.multiply_scale(1.00 - 0.001*step);
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_X)) {
      entity.set_scale({1, 1, 1});
    }

    view.set_position(camera_position);

    model_shader->use();
    lena_texture.bind(*model_shader);
    entity.draw(*model_shader, view);
    //cube.draw(*model_shader);

    renderer.render();
  }

  return 0;
}
