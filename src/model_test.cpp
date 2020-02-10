#include "ZD/OpenGLRenderer.hpp"
#include "ZD/Model.hpp"
#include "ZD/Texture.hpp"
#include "ZD/Shader.hpp"
#include "ZD/ShaderLoader.hpp"

int model_test_main(int, char**)
{
  auto renderer = OGLRenderer();
  renderer.add_window(WindowParameters(Size(800,600), "ZD model test") );

  auto cube = Model(ModelDefault::Cube);
  auto model_shader = ShaderLoader()
      .add(ShaderDefault::CenterModelTextureVertex, GL_VERTEX_SHADER)
      .add(ShaderDefault::CenterModelTextureFragment, GL_FRAGMENT_SHADER)
      .compile();

  while (renderer.is_window_open())
  {
    renderer.clear();
    renderer.update();

    cube.draw(*model_shader);
  }

  return 0;
}
