#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <random>
#include <memory>
#include <string_view>

#include "ZD/ZD.hpp"
#include "ZD/Painter.hpp"
#include "ZD/ScaledPainter.hpp"
#include "ZD/Screen.hpp"
#include "ZD/OpenGLRenderer.hpp"
#include "ZD/File.hpp"
#include "ZD/Input.hpp"

#define W 800
#define H 450

std::shared_ptr<Image> load_image(std::string_view name)
{
  auto img = Image::load(name);
  if (!img)
  {
    printf("Image '%s' cannot be loaded!\n", name.data());
  }
  else
  {
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

auto image_test_main(int, char **) -> int
{
  puts("Image load tests.");
  load_image("images/lena.png");
  load_image("images/lena.png");
  load_image("images/not-exists");
  load_image("wrong-path/not-exists");
  puts("Image load tests complete.");

  puts("Creating renderer...");
  auto renderer = OGLRenderer();
  puts("Adding a window");
  auto window = renderer.add_window({ Size(W, H), "ZDTest" });
  renderer.enable_blend();
  auto input = window->input();

  puts("Creating screens...");
  auto screen = std::make_shared<Screen_GL>(0, 0, W, H);
  window->add_screen(screen);
  auto screen2 = std::make_shared<Screen_GL>(0, 0, W, H);
  window->add_screen(screen2);

  puts("Getting a painter...");
  auto painter = screen->painter();

  puts("Loading an image...");
  auto image = Image::load("images/lena.png");

  if (!image)
  {
    printf("Cannot load test image!");
    return 1;
  }

  puts("Creating an image...");
  auto rnoise = Image::create(Size(W, H), PixelFormat::RGB);
  int x = 10;
  int y = 11;

  auto canvas_image = Image::load("images/user_canvas.png");

  if (FileWatcher::supported)
  {
    auto file = File("images/user_canvas.png", File::Read);
    assert(file.is_open());
    file.set_watch([&canvas_image](
                     const File &file, std::unordered_set<FileEvent> events) {
      printf("File '%s' %zu events.\n", file.get_name().data(), events.size());
      if (events.count(FileEvent::CloseWrite) > 0)
      {
        printf("File modified!\n");
        auto new_image =
          Image::load("images/user_canvas.png", ForceReload::Yes);
        if (new_image)
        {
          canvas_image->set_data(
            new_image->get_data(), new_image->get_size().area());
        }
        else
        {
          printf("New user canvas cannot be loaded!\n");
        }
      }
    });
  }

  Painter rnoise_painter(rnoise);

  auto scaled_image =
    Image::create(Size(W / 4, H / 4), Color(0, 0, 0, 0), PixelFormat::RGBA);
  ScaledPainter scaled_painter(scaled_image, 10);
  scaled_painter.draw_line(0, 20, 20, 0, Color(255, 255, 0));
  scaled_painter.draw_line(0, 0, 20, 30, Color(255, 255, 0));

  puts("Starting main loop...");
  long iteration = 0;
  while (renderer.is_window_open())
  {
    iteration++;

    renderer.update();
    renderer.clear();

    if (input->key(Key::Left))
    {
      puts("Left");
      x--;
    }
    if (input->key(Key::Right))
    {
      x++;
    }
    if (input->key(Key::Up))
    {
      y--;
    }
    if (input->key(Key::Down))
    {
      y++;
    }

    static int rnoise_c = 0;
    if (rnoise_c++ > 10000)
    {
      rnoise_c = 0;
      rnoise->clear();
    }
    for (int i = 0; i < 100; i++)
    {
      rnoise_painter.set_pixel(
        -1000 + r(W + 1000 * 3),
        -1000 + r(H + 1000 * 3),
        Color(r(255), r(255), r(255)));
    }

    painter->clear(Color(0, 0, 0, 255));
    painter->draw_image(x, y, *rnoise, 0.3333, -0.3333);
    painter->draw_rectangle(
      x, y, x + W * 0.3333, y + H * 0.3333, Color(255, 255, 120));

    painter->draw_image(-9999, -9999, *image, 0.4, 0.4);
    painter->draw_image(9999, 9999, *image, 0.4, 0.4);
    painter->draw_image(0, 0, *image, 0.0, 0.0);
    painter->draw_image(x - 50, y + 40, *image, 1.2, 1.2);
    painter->draw_image(x - 80, y + 50, *image, -1.5, -1.5);

    painter->draw_line(10, 10, 300, 300, Color(255, 0, 0));
    painter->draw_line(-100, -50, 300, 320, Color(255, 0, 0));
    painter->draw_line(-10, 10, 3000, 1300, Color(255, 0, 0));

    painter->draw_circle(400, 200, 50, Color(120, 255, 255));
    for (int i = 0; i < 20 / 2; i++)
    {
      int step = 4;
      painter->draw_circle(
        400,
        200 + i * step,
        50 + i * step,
        Color(120, 255 - i * step * 3, 255 - i * step * 3));
    }

    painter->draw_circle(100, 100, 11, Color(255, 120, 20));
    painter->draw_circle(1000, 1000, 1011, Color(255, 120, 20));
    painter->draw_circle(-100, -100, 111, Color(255, 120, 20));
    painter->draw_circle(10, 10, 0, Color(255, 120, 20));
    painter->draw_circle(10, 10, -20, Color(255, 120, 0));

    painter->draw_circle(0, 0, 20, Color(255, 0, 0));
    painter->draw_circle(W, H, 20, Color(255, 0, 0));
    painter->draw_circle(0, H, 20, Color(255, 0, 0));
    painter->draw_circle(W, 0, 20, Color(255, 0, 0));

    painter->draw_rectangle(10, 10, 100, 100, Color(0, 255, 255));
    painter->draw_rectangle(
      -10 + x, -10 + y * 2, 100 + x, 100 + y * 2, Color(0, 255, 255));
    painter->draw_rectangle(-10, -10, 160, 160, Color(120, 55, 55));
    painter->draw_rectangle(-10, 10, 200, 200, Color(120, 55, 55));
    painter->draw_rectangle(10, -10, 300, 300, Color(120, 55, 55));
    painter->draw_rectangle(10, -10, 300, 300, Color(120, 55, 55));

    painter->draw_rectangle(-10, -10, 10, 10, Color(255, 0, 0));
    painter->draw_rectangle(W + -10, -10, W + 10, 10, Color(255, 0, 0));
    painter->draw_rectangle(W + -10, H + -10, W + 10, H + 10, Color(255, 0, 0));
    painter->draw_rectangle(-10, H + -10, 10, H + 10, Color(255, 0, 0));

    painter->draw_image(W - 400, H - 300, *canvas_image, 0.5, 0.5);

    painter->draw_image(W - x, H - y, *scaled_image, 2.0, 2.0);

    screen2->painter()->clear();
    screen2->painter()->draw_circle(W / 2, H / 2, 30, Color(255, 0, 0));
    
    painter->draw_image(x, y, *image);

    renderer.render();
  }

  puts("Saving canvas image to files...");
  screen->image()->save_to_file("test_image");
  screen->image()->save_to_file("test_image.jpg");
  screen->image()->save_to_file("test_image.bmp");
  screen->image()->save_to_file("test_image.tga");

  printf("\nDONE\n");

  return 0;
}
