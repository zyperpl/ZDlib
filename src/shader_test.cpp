#include "ZD/OpenGLRenderer.hpp"
#include "ZD/ModelLoader.hpp"
#include "ZD/ShaderLoader.hpp"
#include "ZD/Image.hpp"
#include "ZD/View.hpp"
#include "ZD/File.hpp"

int shader_test_main(int, char**)
{
  puts("Starting");

  OGLRenderer renderer;
  auto window = renderer.add_window({ Size(600, 600), "zdlib shader test" });

  auto shader_program =
    ShaderLoader()
      .add(ShaderDefault::ScreenTextureVertex, GL_VERTEX_SHADER)
      .add(
        File("images/shader_f.glsl", File::OpenMode::Read), GL_FRAGMENT_SHADER)
      .compile();
  auto screen = std::make_shared<Screen_GL>(shader_program, 0, 0, 600, 600);
  window->add_screen(screen);

  while (renderer.is_window_open())
  {
    renderer.clear();
    renderer.update();

    renderer.render();
  }

  puts("Done");
  return 0;
}
