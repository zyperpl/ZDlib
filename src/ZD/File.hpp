#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <vector>
#include <functional>

#include "FileWatch.hpp"

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

  enum CreateFile
  {
    No = 0,
    Yes = 1
  };

  File(std::string_view file_name, OpenMode mode, CreateFile create = No);
  ~File();

  void rewind();
  std::optional<std::string> read_line(int max_size = FILE_BUF_SIZE);
  std::vector<std::string> read_lines();
  std::vector<uint8_t> read_bytes(int max_size = FILE_BUF_SIZE);
  std::vector<uint8_t> read_all_bytes();

  ssize_t write(std::vector<uint8_t> data);
  ssize_t write(std::string_view str);

  void set_watch(FileCallback callback);
  void remove_watch();

  std::string_view get_name() const { return name; }
  size_t get_size() const { return size; }
  bool is_open() const { return fd != -1; }
  int get_fd() const { return fd; }
protected:
  size_t obtain_size();
private:
  std::string_view name;
  size_t size{0};
  OpenMode mode;

  int fd{-1};

  std::shared_ptr<FileWatcher> file_watcher;
};
