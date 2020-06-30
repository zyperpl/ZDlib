#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

class BufferBuilder
{
public:
  template<typename D>
  size_t add(const D &value)
  {
    buffer.resize(buffer.size() + sizeof(D));
    memcpy(buffer.data() + buffer.size() - sizeof(D), &value, sizeof(D));
    return sizeof(D);
  }

  [[nodiscard]] std::vector<uint8_t> get_buffer() const { return buffer; }

private:
  std::vector<uint8_t> buffer;
};

template<>
size_t BufferBuilder::add<std::string>(const std::string &value);
