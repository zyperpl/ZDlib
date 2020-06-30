#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

class BufferReader
{
public:
  BufferReader(std::vector<uint8_t> buffer)
  : buffer { buffer }
  {
  }
  BufferReader(std::vector<uint8_t> &&buffer)
  : buffer { buffer }
  {
  }

  template<typename D>
  D get()
  {
    if (data_left <= 0)
    {
      errors++;
      return D {};
    }

    D v;
    memcpy(&v, buffer_ptr, sizeof(D));
    buffer_ptr += sizeof(D);
    data_left -= sizeof(D);
    return v;
  }

  bool end() { return data_left <= 0; }
  bool error() { return errors > 0; }

private:
  std::vector<uint8_t> buffer;
  uint8_t *buffer_ptr { buffer.data() };
  size_t data_left { buffer.size() };
  ssize_t errors { 0 };
};

template<>
std::string BufferReader::get();
