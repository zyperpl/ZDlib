#pragma once

namespace ZD
{
  class Size
  {
  public:
    constexpr Size(int w, int h)
    : w { w }
    , h { h }
    {
    }

    constexpr bool is_valid() const { return w >= 0 && h >= 0; }

    constexpr int width() const { return w; }
    constexpr int height() const { return h; }

    constexpr void set_width(int new_width) { w = new_width; }
    constexpr void set_height(int new_height) { h = new_height; }

    constexpr int area() const { return w * h; }

    constexpr bool operator==(const Size &other)
    {
      return other.w == w && other.h == h;
    }

    constexpr bool operator!=(const Size &other)
    {
      return other.w != w || other.h != h;
    }

    constexpr Size &operator-=(const Size &other)
    {
      this->w -= other.w;
      this->h -= other.h;
      return *this;
    }

    constexpr Size &operator+=(const Size &other)
    {
      this->w += other.w;
      this->h += other.h;
      return *this;
    }

    constexpr Size &operator*=(const Size &other)
    {
      this->w *= other.w;
      this->h *= other.h;
      return *this;
    }

    constexpr Size &operator/=(const Size &other)
    {
      if (other.w != 0)
      {
        this->w /= other.w;
      }
      if (other.h != 0)
      {
        this->h /= other.h;
      }
      return *this;
    }

    constexpr bool operator>(const Size &other)
    {
      return w > other.w && h > other.h;
    }

    constexpr Size &operator++()
    {
      this->w++;
      this->h++;
      return *this;
    }

    constexpr Size &operator--()
    {
      this->w--;
      this->h--;
      return *this;
    }

  private:
    int w { 0 };
    int h { 0 };
  };

} // namespace ZD
