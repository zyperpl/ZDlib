#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <vector>

#define FILE_BUF_SIZE 4096

class File
{
public:
  enum OpenMode
  {
    Read = 1,
    Write = 2,
    ReadWrite = 3
  };

  File(std::string_view file_name, OpenMode mode);
  ~File();

  void rewind();
  std::optional<std::string> read_line(int max_size = FILE_BUF_SIZE);
  std::vector<std::string> read_lines();
  std::vector<uint8_t> read_bytes(int max_size = FILE_BUF_SIZE);
  std::vector<uint8_t> read_all_bytes();

  std::string_view get_name() const { return name; }
  size_t get_size() const { return size; }
  bool is_open() const { return fd != -1; }
protected:
  size_t obtain_size();
private:
  std::string_view name;
  size_t size{0};
  OpenMode mode;

  int fd{-1};
};
