#pragma once

#include <string>
#include <string_view>
#include <vector>

class File
{
public:
  File(std::string_view file_name);
  std::vector<std::string> read_lines();
  std::vector<uint8_t> read_bytes();
private:
  std::string_view file_name;
};
