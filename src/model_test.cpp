#include "ZD/OpenGLRenderer.hpp"
#include "ZD/Model.hpp"
#include "ZD/Texture.hpp"
#include "ZD/Shader.hpp"
#include "ZD/ShaderLoader.hpp"
#include "ZD/Entity.hpp"
#include "ZD/View.hpp"
#include "ZD/Input.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define WIDTH  1920 / 3
#define HEIGHT 1080 / 3

static int r(int m = 100)
{
  static std::random_device rd;
  static std::mt19937 g(rd());
  std::uniform_int_distribution<int> dist(0, m);
  return dist(g);
}


int model_test_main(int, char **)
{
  puts("Creating renderer with window");
  auto renderer = OGLRenderer();
  auto window =
    renderer.add_window(WindowParameters(Size(WIDTH, HEIGHT), "ZD model test"));
  renderer.clear_background_color(Color(10, 10, 20));
  puts("Enabling cull face");
  renderer.enable_cull_face();
  puts("Enabling depth test");
  renderer.enable_depth_test();
  auto input = window->input();

  puts("Loading a model");
  auto cube = Model(ModelDefault::Cube);
  puts("Loading a shader");
  auto model_shader =
    ShaderLoader()
      .add(ShaderDefault::CenterModelTextureVertex, GL_VERTEX_SHADER)
      .add(ShaderDefault::CenterModelTextureFragment, GL_FRAGMENT_SHADER)
      .compile();

  puts("Creating a texture");
  auto lena_texture = std::make_shared<Texture>(Image::load("images/lena.png"));

  auto tank_texture =
    std::make_shared<Texture>(Image::load("images/propane_tank_red.png"));
  auto tank = Model("images/propane_tank.obj");
  tank.add_texture(tank_texture);

  auto crate_texture =
    std::make_shared<Texture>(Image::load("images/crate_1.jpg"));
  auto crate = Model("images/Crate1.obj");
  crate.add_texture(crate_texture);

  puts("Creating special texture..");
  auto custom_texture = std::make_shared<Texture>();
  auto crate2 = Model("images/Crate1.obj");
  crate2.add_texture(custom_texture);

  puts("Creating an entity...");
  auto entity = Entity({ 0.2, 0.2, 0.2 }, { 0, 0, 0 }, { .4, .4, .4 });
  entity.add_model(tank);
  entity.add_model(crate);

  puts("Creating the second entity...");
  const int MONS = 4;
  Entity monoliths[MONS];
  for (ssize_t i = 0; i < MONS; ++i)
  {
    monoliths[i] = Entity({ 3.0 + (i * 3.), 0.2, 0.2 }, { 0, 0, 0 }, { 1., 4., 1. });
    monoliths[i].add_model(crate2);
  }

  puts("Creating View");
  auto view_perspective = View(
    Camera::PerspectiveParameters(
      Camera::Fov(glm::radians(85.0f)),
      float(WIDTH) / float(HEIGHT),
      Camera::ClippingPlane(0.1f, 1000.f)),
    { 3.0, 0.0, -1.0 });

  auto view_orthographic = View(
    Camera::OrtographicParameters(
      Camera::OrthographicBox(-10.f, 10.f, -10.f, 10.f),
      Camera::ClippingPlane(0.1f, 10000.f)),
    { -1.0, 0.0, -1.0 });
  View *view = &view_perspective;

  glm::vec3 camera_position(-1, 0, -1);

  puts("Starting main loop...");
  while (renderer.is_window_open())
  {
    static long iteration = 0;
    iteration++;

    renderer.update();
    renderer.clear();

    float step = 1.0;
    if (input->key(Key::LeftShift))
      step *= 10.;
    if (input->key(Key::LeftControl))
      step *= 10.;

    if (input->key(Key::A))
    {
      camera_position.x -= .0001 * step;
    }
    if (input->key(Key::D))
    {
      camera_position.x += .0001 * step;
    }
    if (input->key(Key::W))
    {
      camera_position.y += .0001 * step;
    }
    if (input->key(Key::S))
    {
      camera_position.y -= .0001 * step;
    }
    if (input->key(Key::E))
    {
      camera_position.z += .0001 * step;
    }
    if (input->key(Key::Q))
    {
      camera_position.z -= .0001 * step;
    }

    if (input->key(Key::P))
    {
      view = &view_orthographic;
    }
    else
    {
      view = &view_perspective;
    }

    if (input->key(Key::R))
    {
      entity.add_rotation({ 0.001 * step, 0, 0 });
    }
    if (input->key(Key::F))
    {
      entity.add_rotation({ 0, 0.001 * step, 0 });
    }
    if (input->key(Key::V))
    {
      entity.add_rotation({ 0, 0, 0.001 * step });
    }
    if (input->key(Key::C))
    {
      entity.multiply_scale(1.00 + 0.001 * step);
    }
    if (input->key(Key::Z))
    {
      entity.multiply_scale(1.00 - 0.001 * step);
    }
    if (input->key(Key::X))
    {
      entity.set_scale({ 1, 1, 1 });
    }

    if (iteration % 12 == 0)
    {
      int w = 5+r(120);
      int h = 2+r(100);
      std::shared_ptr<Image> custom_img = Image::create(Size(w, h));
      for (int i = 0; i < 100; i++)
      {
        custom_img->set_pixel(r(w-1), r(h-1), Color(r(250), r(255), 55+r(200)));
      }
      custom_texture->set_image(custom_img);
    }

    view->set_position(camera_position);
    entity.draw(*model_shader, *view);
    for (size_t i = 0; i < MONS; i++)
    {
      monoliths[i].draw(*model_shader, *view);
    }
    renderer.render();

    if (input->key(Key::Escape))
    {
      renderer.remove_window(0);
    }
  }
  puts("Done.");
  return 0;
}
