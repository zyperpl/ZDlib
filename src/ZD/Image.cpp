#include <cstring>
#include <memory>
#include <unordered_map>

#include "Image.hpp"
#include "ImageLoader.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3rd/stb_image_write.h"

#define JPEG_QUALITY 100

#pragma GCC optimize("O3")
namespace ZD
{
  std::shared_ptr<Image> Image::load(
    std::string_view file_name, ForceReload reload)
  {
    return ImageLoader::load(file_name, reload);
  }

  std::shared_ptr<Image> Image::create(
    const Size &size, PixelFormat::Type format)
  {
    std::shared_ptr<Image> image =
      std::shared_ptr<Image>(new Image(size, format));
    return image;
  }

  std::shared_ptr<Image> Image::create(
    const Size &size, const Color &color, PixelFormat::Type format)
  {
    std::shared_ptr<Image> image =
      std::shared_ptr<Image>(new Image(size, format));
    image->clear(color);
    return image;
  }

  Image::Image(const Size &size, PixelFormat::Type format)
  : size { size }
  , format { format }
  {
    size_t data_size = size.area();
    this->data.reset(new uint32_t[data_size]);

    memset(data.get(), 0, data_size * sizeof(uint32_t));
  }

  bool Image::save_to_file(std::string file_name)
  {
    size_t file_ext_pos = file_name.find_last_of(".");
    std::string file_ext = "png";
    if (file_ext_pos != std::string::npos)
    {
      file_ext = file_name.substr(file_ext_pos + 1);
    }
    else
    {
      file_name += ".";
      file_name += file_ext;
    }
    printf(
      "Saving %p to file '%s' (ext=%s).\n",
      (void*)(this),
      file_name.data(),
      file_ext.data());

    const auto w = size.width();
    const auto h = size.height();
    const auto comp = PixelFormat::get_components_num(format);

    uint8_t *u8_data = ImageLoader::u32_to_u8(data.get(), w, h, comp);

    bool ret = false;
    if (file_ext == "png")
    {
      ret = stbi_write_png(
              file_name.data(), w, h, comp, u8_data, size.width() * comp) > 0;
    }
    else if (file_ext == "bmp")
    {
      ret = stbi_write_bmp(file_name.data(), w, h, comp, u8_data) > 0;
    }
    else if (file_ext == "tga")
    {
      // TODO: TGA not always works
      ret = stbi_write_tga(file_name.data(), w, h, comp, u8_data) > 0;
    }
    else if (file_ext == "jpg")
    {
      ret =
        stbi_write_jpg(file_name.data(), w, h, comp, u8_data, JPEG_QUALITY) > 0;
    }

    delete[] u8_data;

    assert(ret);

    return ret;
  }
} // namespace ZD
