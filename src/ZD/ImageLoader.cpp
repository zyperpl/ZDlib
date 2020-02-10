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

static std::unordered_map<std::string_view, std::shared_ptr<Image>> loaded_images;

struct LoadedImage
{
  std::string file_name;
  int width;
  int height;
  PixelFormat::Type format;
  uint32_t *data;
};

uint32_t *convert_to_u32_bitmap(uint8_t *bitmap, 
                                int width, 
                                int height, 
                                int channels)
{
  const int size = width * height;
  uint32_t *data = new uint32_t[size];


  #pragma omp parallel for
  for (ssize_t i = 0; i < size; i++)
  {
    uint8_t r = 0, g = 0, b = 0, a = 255;
    const uint8_t *px = bitmap + i * channels;
    if (channels > 0) r = px[0];
    if (channels > 1) g = px[1];
    if (channels > 2) b = px[2];
    if (channels > 3) a = px[3];

    data[i] = Color(r, g, b, a).value();
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

  auto u32_data = convert_to_u32_bitmap(data, loaded.width, loaded.height, CHANNEL_NUM); 

  stbi_image_free(data);

  loaded.file_name = file_name;
  loaded.format = PixelFormat::Type::RGBA;
  loaded.data = u32_data;
  return loaded;
}

std::optional<std::shared_ptr<Image>> find_in_loaded(std::string_view path)
{
  auto name_image_pair = loaded_images.find(path);
  if (name_image_pair != loaded_images.end()) 
  {
    printf("Found in loaded!\n");
    return name_image_pair->second;
  }

  return std::nullopt;
}

std::shared_ptr<Image> ImageLoader::load(std::string_view path)
{
  Image *image = NULL;
  if (auto already_loaded = find_in_loaded(path))
  {
    return already_loaded.value();
  }

  if (auto loaded_data = load_image_via_stbi(path))
  {
    Size size(loaded_data->width, loaded_data->height);
    image = new Image(size, loaded_data->format);
    image->path = loaded_data->file_name;
    image->data = std::unique_ptr<uint32_t[]>(loaded_data->data);
  }

  std::shared_ptr<Image> image_ptr(image);

  if (image_ptr) {
    loaded_images.emplace(path, image_ptr);
  }

  return image_ptr;
}
