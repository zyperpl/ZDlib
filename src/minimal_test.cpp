#include "ZD/OpenGLRenderer.hpp"

int minimal_test_main(int, char**)
{
  puts("Starting");

  puts("Creating renderer...");
  OGLRenderer renderer;
  puts("Adding window...");
  renderer.add_window({Size(640, 480), "zdlib minimal test"});
  
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
    renderer.get_window().kill();
  }

  puts("Done");
  return 0;
}
