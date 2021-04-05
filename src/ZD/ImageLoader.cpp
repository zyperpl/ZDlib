#include <cstdio>
#include <cstdlib>
#include <memory>
#include <optional>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PSD
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_FAILURE_USERMSG
#include "3rd/stb_image.h"

#include "ImageLoader.hpp"
#include "Image.hpp"

#pragma GCC optimize("O3")
namespace ZD
{
  static std::unordered_map<std::string, std::shared_ptr<Image>> loaded_images;

  struct LoadedImage
  {
    std::string file_name;
    int width;
    int height;
    PixelFormat::Type format;
    uint32_t *data;
  };

  uint32_t *ImageLoader::u8_to_u32(uint8_t *bitmap, int width, int height, int channels)
  {
    const int size = width * height;
    uint32_t *data = new uint32_t[size];

    for (ssize_t i = 0; i < size; i++)
    {
      uint8_t r = 0, g = 0, b = 0, a = 255;
      const uint8_t *px = bitmap + i * channels;
      r = px[0];
      if (channels > 1)
        g = px[1];
      if (channels > 2)
        b = px[2];
      if (channels > 3)
        a = px[3];

      data[i] = Color(r, g, b, a).value();
    }

    return data;
  }

  uint8_t *ImageLoader::u32_to_u8(uint32_t *bitmap, int width, int height, int channels)
  {
    const int size = width * height * channels;
    uint8_t *data = new uint8_t[size];

    for (ssize_t i = 0; i < width * height; i++)
    {
      const uint32_t v = bitmap[i];
      const Color color = Color::from_value(v);

      const ssize_t d_idx = i * channels;
      data[d_idx + 0] = color.red();
      if (channels >= 2)
        data[d_idx + 1] = color.green();
      if (channels >= 3)
        data[d_idx + 2] = color.blue();
      if (channels >= 4)
        data[d_idx + 3] = color.alpha();
    }

    return data;
  }

  std::optional<LoadedImage> load_image_via_stbi(std::string_view file_name)
  {
    const int CHANNEL_NUM = 4;

    LoadedImage loaded;
    int channels = -1;
    stbi_uc *data = stbi_load(file_name.data(), &loaded.width, &loaded.height, &channels, CHANNEL_NUM);

    if (!data)
    {
      auto err = stbi_failure_reason();
      fprintf(stderr, "stbi failure: %s\n", err);
      return std::nullopt;
    }

    auto u32_data = ImageLoader::u8_to_u32(data, loaded.width, loaded.height, CHANNEL_NUM);

    stbi_image_free(data);

    loaded.file_name = file_name;
    loaded.format = PixelFormat::Type::BGRA;
    loaded.data = u32_data;
    return loaded;
  }

  static std::optional<std::shared_ptr<Image>> find_in_loaded(std::string path)
  {
    auto name_image_pair = loaded_images.find(path);
    if (name_image_pair != loaded_images.end())
    {
      //printf("Image %s found in loaded!\n", path.c_str());
      return name_image_pair->second;
    }

    return std::nullopt;
  }

  std::shared_ptr<Image> ImageLoader::load(std::string path, ForceReload reload)
  {
    Image *image = NULL;
    if (reload != ForceReload::Yes)
    {
      if (auto already_loaded = find_in_loaded(path))
      {
        return *already_loaded;
      }
    }

    if (auto loaded_data = load_image_via_stbi(path))
    {
      Size size(loaded_data->width, loaded_data->height);
      image = new Image(size, loaded_data->format);
      image->path = loaded_data->file_name;
      image->data = std::unique_ptr<uint32_t[]>(loaded_data->data);
    }

    std::shared_ptr<Image> image_ptr(image);

    if (image_ptr)
    {
      loaded_images.emplace(path, image_ptr);
    }

    return image_ptr;
  }
} // namespace ZD
