#include "ScaledPainter.hpp"

ScaledPainter::ScaledPainter(std::shared_ptr<Image> image, float x_scaler, float y_scaler)
  : Painter(image), x_scaler{x_scaler}, y_scaler{y_scaler}
{}
 
ScaledPainter::ScaledPainter(std::shared_ptr<Image> image, float scaler)
  : ScaledPainter(image, scaler, scaler)
{}
 
ScaledPainter::ScaledPainter(std::shared_ptr<uint32_t[]> data, const Size &size, PixelFormat::Type format, float x_scaler, float y_scaler)
  : Painter(data, size, format), x_scaler{x_scaler}, y_scaler{y_scaler}
{}

ScaledPainter::ScaledPainter(std::shared_ptr<uint32_t[]> data, const Size &size, PixelFormat::Type format, float scaler)
  : ScaledPainter(data, size, format, scaler, scaler)
{}


void ScaledPainter::set_pixel(int x, int y, const Color &color)
{
  x = this->scale_h(x);
  y = this->scale_v(y);
  Painter::set_pixel(x, y, color);
}

void ScaledPainter::draw_image(int x, int y, const Image &image)
{
  x = this->scale_h(x);
  y = this->scale_v(y);
  Painter::draw_image(x, y, image, (double)(this->x_scaler), (double)(this->y_scaler));
}

void ScaledPainter::draw_image(int x, int y, const Image &image, double scale_x, double scale_y)
{
  x = this->scale_h(x);
  y = this->scale_v(y);
  scale_x = this->scale_h(scale_x);
  scale_y = this->scale_v(scale_y);
  Painter::draw_image(x, y, image, scale_x, scale_y);
}

void ScaledPainter::draw_image(int x, int y, const Image &image, int width, int height)
{
  x = this->scale_h(x);
  y = this->scale_v(y);
  width = this->scale_h(width);
  height = this->scale_v(height);
  Painter::draw_image(x, y, image, width, height);
}

void ScaledPainter::draw_line(int x1, int y1, int x2, int y2, const Color &color)
{
  x1 = this->scale_h(x1);
  x2 = this->scale_h(x2);
  y1 = this->scale_v(y1);
  y2 = this->scale_v(y2);
  Painter::draw_line(x1, y1, x2, y2, color);
}

void ScaledPainter::draw_rectangle(int x1, int y1, int x2, int y2, const Color &color)
{
  x1 = this->scale_h(x1);
  x2 = this->scale_h(x2);
  y1 = this->scale_v(y1);
  y2 = this->scale_v(y2);
  Painter::draw_rectangle(x1, y1, x2, y2, color);
}

void ScaledPainter::draw_circle(int x, int y, int radius, const Color &color)
{
  x = this->scale_h(x);
  x = this->scale_h(x);
  radius = this->scale_v(radius);
  Painter::draw_circle(x, y, radius, color);
}
