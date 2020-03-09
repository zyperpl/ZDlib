#pragma once

#include "Image.hpp"
#include "Size.hpp"
#include <memory>
class Renderer;
class Image;

#include "Color.hpp"

class Painter
{
public:
  Painter(std::shared_ptr<Image> image);

  virtual void set_pixel(const int x, const int y, const Color &color);
  virtual void draw_image(const int x, const int y, const Image &image);
  virtual void draw_image(
    const int x, const int y, const Image &image, double scale_x,
    double scale_y);
  virtual void draw_image(
    const int x, const int y, const Image &image, const int width,
    const int height);
  virtual void draw_line(
    const int x1, const int y1, const int x2, const int y2, const Color &color);
  virtual void draw_rectangle(
    const int x1, const int y1, const int x2, const int y2, const Color &color);
  virtual void draw_circle(
    const int x, const int y, const int radius, const Color &color);
  inline void clear(const Color &c = Color(0)) { target->clear(c); }
  inline void fill(const Color &c) { target->clear(c); }

  const Color get_pixel(const int x, const int y) const
  {
    return target->get_pixel(x, y);
  }

  std::shared_ptr<Image> get_target_image() { return this->target; }

private:
  std::shared_ptr<Image> target;
};
