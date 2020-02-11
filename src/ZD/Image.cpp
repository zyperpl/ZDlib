#include <cstring>
#include <memory>
#include <unordered_map>

#include "Image.hpp"
#include "ImageLoader.hpp"

#pragma GCC optimize ("O3")

std::shared_ptr<Image> Image::load(std::string_view file_name)
{
  return ImageLoader::load(file_name);
}


std::shared_ptr<Image> Image::create(const Size &size, PixelFormat::Type format)
{
  std::shared_ptr<Image> image = std::shared_ptr<Image>(new Image(size, format) );
  return image;
}

Image::Image(const Size &size, PixelFormat::Type format)
  : size{size}
  , format{format}
{
  size_t data_size = size.area();
  this->data.reset(new uint32_t[data_size]);

  memset(data.get(), 0, data_size*sizeof(uint32_t));
}
