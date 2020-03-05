#include "ZD/OpenGLRenderer.hpp"

int minimal_test_main(int, char**)
{
  puts("Starting");

  puts("Creating renderer...");
  OGLRenderer renderer;
  puts("Adding window...");
  auto window = renderer.add_window({ Size(800, 600), "zdlib minimal test" });

  puts("Starting main loop...");
  while (renderer.is_window_open())
  {
    puts("Clearing screen...");
    renderer.clear();
    puts("Updating renderer...");
    renderer.update();

    puts("Rendering...");
    renderer.render();

    puts("Killing window...");
    window->kill();
  }

  puts("Done");
  return 0;
}
