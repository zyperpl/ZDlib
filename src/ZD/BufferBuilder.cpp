#include "BufferBuilder.hpp"

namespace ZD
{
  template<>
  size_t BufferBuilder::add<std::string>(const std::string &value)
  {
    const auto &size = sizeof(char) * value.size() + 1;
    buffer.resize(buffer.size() + size);
    memcpy(buffer.data() + buffer.size() - size, value.data(), size);
    return size;
  }

} // namespace ZD
