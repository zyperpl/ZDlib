#pragma once

#include <cassert>
#include <cstdint>
#include <cstdio>

typedef unsigned char u8;

struct PixelFormat
{
  enum Type
  {
    Indexed,
    Gray,
    GrayAlpha, 
    RGB,
    RGBA,
    BGR,
    BGRA,
    Invalid
  };
  static int get_components_num(Type format) {
    switch (format) {
      case Type::Indexed:
      case Type::Gray:
        return 1; 

      case Type::GrayAlpha:
        return 2;

      case Type::RGB:
      case Type::BGR:
        return 3;

      case Type::RGBA:
      case Type::BGRA:
        return 4;

      case Type::Invalid:
        assert(false);
    }

    return 3;
  }
};

class Color
{
public:



  Color(uint32_t v)
    : color_value{v}
  {}

  Color(u8 red, u8 green, u8 blue) 
  {
    set_value(red, green, blue);
  }

  Color(u8 red, u8 green, u8 blue, u8 alpha)
  {
    set_value(red, green, blue, alpha);
  }

  static Color from_floats(float red, float green, float blue, float alpha = 1.0)
  {
    u8 r = red   * 255;
    u8 g = green * 255;
    u8 b = blue  * 255;
    u8 a = alpha * 255;

    return Color(r, g, b, a);
  }

  inline void set_value(u8 r, u8 g, u8 b, u8 a = 255)
  {
    color_value = ((r << 24) | (g << 16) | (b << 8) | (a));
  }

  inline uint32_t value() const { return color_value; }

  inline u8 red  () const { return (color_value >> 24) & 0xff; }
  inline u8 green() const { return (color_value >> 16)  & 0xff; }
  inline u8 blue () const { return (color_value >> 8)  & 0xff; }
  inline u8 alpha() const { return (color_value >> 0) & 0xff; }

  inline void print() const { printf("Color (%8x): %3d,%3d,%3d,%3d", value(), red(), green(), blue(), alpha()); }

private:
  uint32_t color_value{0};

};
