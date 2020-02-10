#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <random>
#include <memory>
#include <string_view>

#include "ZD/ZD.hpp"
#include "ZD/Painter.hpp"
#include "ZD/OpenGLRenderer.hpp"

#define W 800
#define H 450

std::shared_ptr<Image> load_image(std::string_view name)
{
  auto img = Image::load(name);
  if (!img) {
    printf("Image '%s' cannot be loaded!\n", name.data());
  } else {
    printf("Image '%s' loaded!\n", name.data());
    img->get_pixel(0, 0).print();
  }
  return img;
}

static int r(int m = 100)
{
  static std::random_device rd;
  static std::mt19937 g(rd());
  std::uniform_int_distribution<int> dist(0, m);
  return dist(g);
}

auto main(int, char**)->int 
{
  load_image("images/lena.png");
  load_image("images/lena.png");
  load_image("images/not-exists");
  load_image("wrong-path/not-exists");

  auto renderer = OGLRenderer();
  renderer.add_window({Size(W, H), "ZDTest"});

  Painter painter(renderer.get_main_screen_image());

  auto image = Image::load("images/lena.png");

  if (!image) {
    printf("Cannot load test image!");
    return 1;
  }

  auto rnoise = Image::create(Size(W, H), PixelFormat::RGB);
  int x = 10;
  int y = 11;

  Painter rnoise_painter(rnoise);

  long iteration = 0;
  while (renderer.is_window_open())
  {
    iteration ++;
    
    renderer.update();
    renderer.clear();
  
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT)) x--;
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_RIGHT)) x++;
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_UP)) y--;
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_DOWN)) y++;

    static int rnoise_c = 0;
    if (rnoise_c++ > 10000) {
      rnoise_c = 0;
      rnoise->clear();
    }
    for (int i = 0; i < 100; i++)
    {
      rnoise_painter.set_pixel(
          -1000+r(W+1000*3), 
          -1000+r(H+1000*3),
          Color(r(255), r(255), r(255)) ); 
    }

    painter.draw_image(x, y, *rnoise, 0.3333, -0.3333);
    painter.draw_rectangle(x, y, x+W*0.3333, y+H*0.3333, Color(255, 255, 120));

    painter.draw_image(x, y, *image, 0.4, 0.4);
    painter.draw_image(-9999, -9999, *image, 0.4, 0.4);
    painter.draw_image(9999, 9999, *image, 0.4, 0.4);
    painter.draw_image(0, 0, *image, 0.0, 0.0);
    painter.draw_image(x - 50, y + 40, *image, 1.2, 1.2);
    painter.draw_image(x - 80, y + 50, *image, -1.5, -1.5);

    painter.draw_line(10, 10, 300, 300, Color(255, 0, 0));
    painter.draw_line(-100, -50, 300, 320, Color(255, 0, 0));
    painter.draw_line(-10, 10, 3000, 1300, Color(255, 0, 0));

    painter.draw_circle(400, 200, 50, Color(120, 255, 255));
    for (int i = 0; i < 20/2; i++)
    {
      int step = 4;
      painter.draw_circle(400, 200+i*step, 50+i*step, Color(120, 255-i*step*3, 255-i*step*3));
    }

    painter.draw_circle(100, 100, 11, Color(255, 120, 20));
    painter.draw_circle(1000, 1000, 1011, Color(255, 120, 20));
    painter.draw_circle(-100, -100, 111, Color(255, 120, 20));
    painter.draw_circle(10, 10, 0, Color(255, 120, 20));
    painter.draw_circle(10, 10, -20, Color(255, 120, 0));
    
    painter.draw_circle(0, 0, 20, Color(255, 0, 0));
    painter.draw_circle(W, H, 20, Color(255, 0, 0));
    painter.draw_circle(0, H, 20, Color(255, 0, 0));
    painter.draw_circle(W, 0, 20, Color(255, 0, 0));

    painter.draw_rectangle(10, 10, 100, 100, Color(0, 255, 255));
    painter.draw_rectangle(-10+x, -10+y*2, 100+x, 100+y*2, Color(0, 255, 255));
    painter.draw_rectangle(-10, -10, 160, 160, Color(120, 55, 55));
    painter.draw_rectangle(-10, 10, 200, 200, Color(120, 55, 55));
    painter.draw_rectangle(10, -10, 300, 300, Color(120, 55, 55));
    painter.draw_rectangle(10, -10, 300, 300, Color(120, 55, 55));

    painter.draw_rectangle(-10, -10, 10, 10, Color(255, 0, 0));
    painter.draw_rectangle(W+-10, -10, W+10, 10, Color(255, 0, 0));
    painter.draw_rectangle(W+-10, H+-10, W+10, H+10, Color(255, 0, 0));
    painter.draw_rectangle(-10, H+-10, 10, H+10, Color(255, 0, 0));

    renderer.render();
  }

  printf("\nDONE\n");
}
