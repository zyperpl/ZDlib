#pragma once

#include "Painter.hpp"
#include "Image.hpp"

namespace ZD
{
  class ScaledPainter : public Painter
  {
  public:
    ScaledPainter(std::shared_ptr<Image> image, float x_scaler, float y_scaler);
    ScaledPainter(std::shared_ptr<Image> image, float scaler);

    void set_pixel(int x, int y, const Color &color);
    void draw_image(int x, int y, const Image &image);
    void draw_image(
      int x, int y, const Image &image, double scale_x, double scale_y);
    void draw_image(int x, int y, const Image &image, int width, int height);
    void draw_line(int x1, int y1, int x2, int y2, const Color &color);
    void draw_rectangle(int x1, int y1, int x2, int y2, const Color &color);
    void draw_circle(int x, int y, int radius, const Color &color);

    void set_x_scaler(float n) { x_scaler = n; }
    void set_y_scaler(float n) { y_scaler = n; }

    float get_x_scaler() const { return x_scaler; }
    float get_y_scaler() const { return y_scaler; }

  protected:
    template<typename T>
    T scale_h(T v)
    {
      return v * x_scaler;
    }

    template<typename T>
    T scale_v(T v)
    {
      return v * x_scaler;
    }

  private:
    float x_scaler { 1.0 };
    float y_scaler { 1.0 };
  };

} // namespace ZD
