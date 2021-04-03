#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "File.hpp"

namespace ZD
{
  struct ModelData
  {
    std::vector<unsigned int> indices;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> uvs;
  };

  class ModelLoader
  {
  public:
    static std::optional<std::vector<ModelData>> load(std::string_view file_name, ForceReload reload = ForceReload::No);
  };

} // namespace ZD
