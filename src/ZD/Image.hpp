#pragma once


#include <cassert>
#include <memory>
#include <string>
#include <string.h>
#include <string_view>

#include "Size.hpp"
#include "Color.hpp"

enum class ForceReload
{
  No = 0,
  Yes = 1
};

class Image
{
public:
  static std::shared_ptr<Image> load(std::string_view file_name, ForceReload reload = ForceReload::No);
  static std::shared_ptr<Image> create(const Size &size, PixelFormat::Type format = PixelFormat::RGB);
  static std::shared_ptr<Image> create(const Size &size, const Color &color, PixelFormat::Type format = PixelFormat::RGB);

  bool is_empty() const { return !size.is_valid(); }
  bool is_null()  const { return data == NULL; }
  bool is_valid() const { return !is_empty() && !is_null(); }
  PixelFormat::Type get_format() const { return format; }
  Size get_size() const { return size; }
  int width() const { return size.width(); }
  int height() const { return size.height(); }
  const uint32_t *get_data() const { return data.get(); }

  inline Color get_pixel(int x, int y) const {
    return data[x + y*size.width()]; 
  }

  void set_data(const uint32_t *other_data, size_t area) {
    memcpy(data.get(), other_data, area*sizeof(uint32_t) );
  }

  void set_pixel(int x, int y, Color color) {
    data[x + y*size.width()] = color.value(); 
  }

  void clear(Color color = Color(0))
  {
    uint32_t v = color.value();
    if (v < 255)
    {
      memset(data.get(), v, size.area() * sizeof(uint32_t));
    } else
    {
      std::fill(data.get(), data.get()+size.area(), v);
    }
  }

  inline void print() const {
    printf("Image %p (%d; %d)\n{\n", this, size.width(), size.height());
    for (int y = 0; y < size.height(); y++)
    {
      printf(" ");
      for (int x = 0; x < size.width(); x++)
      {
        printf("%x ", data[x+y*size.width()]);
      }
      printf("\n");
    }
    printf("}\n");
  }

  bool save_to_file(std::string file_name);

private:
  Image() = default;
  Image(const Size &size, PixelFormat::Type format);

  std::string path;
  Size size{0,0};
  PixelFormat::Type format{PixelFormat::Invalid};
  std::shared_ptr<uint32_t[]> data;

  friend class ImageLoader;
  friend class Painter;
};
