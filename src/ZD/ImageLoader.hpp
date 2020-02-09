#pragma once

#include <memory>
#include <string_view>

class Image;

class ImageLoader
{
public:
  static std::shared_ptr<Image> load(std::string_view path);
};
