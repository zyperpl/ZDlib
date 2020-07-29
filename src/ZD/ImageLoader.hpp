#pragma once

#include <memory>
#include <string_view>

namespace ZD
{
  class Image;
  enum class ForceReload;

  class ImageLoader
  {
  public:
    static std::shared_ptr<Image> load(
      std::string_view path, ForceReload reload);

    static uint32_t *u8_to_u32(
      uint8_t *bitmap, int width, int height, int channels);
    static uint8_t *u32_to_u8(
      uint32_t *bitmap, int width, int height, int channels);
  };

} // namespace ZD
