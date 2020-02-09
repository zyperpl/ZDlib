#ifndef ZDLIB_DEFINED
#define ZDLIB_DEFINED
#pragma once

#include "Window.hpp"
#include "Renderer.hpp"
#include "Painter.hpp"
#include "Image.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Color.hpp"

namespace ZD
{
  void create_window(int width, int height, std::string_view title);
}

#endif
