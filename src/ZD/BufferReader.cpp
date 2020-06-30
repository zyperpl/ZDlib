#include "BufferReader.hpp"

template<>
std::string BufferReader::get()
{
  std::string str { "" };
  if (data_left <= 0)
    errors++;
  while (data_left > 0)
  {
    uint8_t ch = get<uint8_t>();
    if (ch == '\0')
      break;
    str += ch;
  }
  return str;
}
