#pragma once

class Size
{
public:
  Size(int w, int h)
  : w { w }
  , h { h }
  {
  }

  bool is_valid() const { return w >= 0 && h >= 0; }

  int width() const { return w; }
  int height() const { return h; }

  void set_width(int new_width) { w = new_width; }
  void set_height(int new_height) { h = new_height; }

  int area() const { return w * h; }

  bool operator==(const Size &other) { return other.w == w && other.h == h; }

  bool operator!=(const Size &other) { return other.w != w || other.h != h; }

  Size &operator-=(const Size &other)
  {
    this->w -= other.w;
    this->h -= other.h;
    return *this;
  }

  Size &operator+=(const Size &other)
  {
    this->w += other.w;
    this->h += other.h;
    return *this;
  }

  Size &operator*=(const Size &other)
  {
    this->w *= other.w;
    this->h *= other.h;
    return *this;
  }

  Size &operator/=(const Size &other)
  {
    if (other.w != 0) { this->w /= other.w; }
    if (other.h != 0) { this->h /= other.h; }
    return *this;
  }

  bool operator>(const Size &other) { return w > other.w && h > other.h; }

  Size &operator++()
  {
    this->w++;
    this->h++;
    return *this;
  }

  Size &operator--()
  {
    this->w--;
    this->h--;
    return *this;
  }

private:
  int w { 0 };
  int h { 0 };
};
