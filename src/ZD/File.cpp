#include "File.hpp"

#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <stdio.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

File::File(std::string_view file_name, OpenMode mode)
  : name{file_name}, mode{mode}
{
  int oflag = 0;
  switch (mode)
  {
    case Read: oflag = O_RDONLY; break;
    case Write: oflag = O_WRONLY; break;
    case ReadWrite: oflag = O_RDWR; break;
  }
  fd = open(file_name.data(), oflag); 

  if (fd == -1)
  {
    fprintf(stderr, "Cannot open file '%s'!\n", file_name.data());
  } else
  {
    obtain_size();
  }
}

File::~File()
{
  close(fd);
}

void File::rewind()
{
  lseek(fd, 0, SEEK_SET);
  assert(lseek(fd, 0, SEEK_CUR) == 0);
}

size_t File::obtain_size()
{
  struct stat sb;
  if (fstat(fd, &sb) == -1)
  {
    perror("obtain_size fstat");
    fprintf(stderr, "Cannot fstat file '%s'!\n", name.data());
  }

  size = sb.st_size; 
  return sb.st_size;
}

std::optional<std::string> File::read_line(int max_size)
{
  std::unique_ptr<uint8_t[]> buf(new uint8_t[max_size]);
  const uint8_t *beg = buf.get();
  size_t readed = 0;
  while ((readed = read(fd, buf.get(), max_size)) > 0)
  {
    uint8_t *p = (uint8_t*)memchr(buf.get(), '\n', readed);
    if (p) {
      size_t offset = p-buf.get();
      //printf("buf=%p p=%p offset=%lu\n", buf.get(), p, offset);
      lseek(fd, -(readed-offset)+1, SEEK_CUR);
      return std::string(buf.get(), buf.get()+offset);
    }

    beg += readed;
  }
  
  return {};
}

std::vector<std::string> File::read_lines()
{
  rewind();
  assert(lseek(fd, 0, SEEK_CUR) == 0);

  std::vector<std::string> strings;
  
  std::unique_ptr<uint8_t[]> buf(new uint8_t[FILE_BUF_SIZE]);
  const uint8_t *beg = buf.get();
  size_t readed = 0;
  while ((readed = read(fd, buf.get(), FILE_BUF_SIZE)) > 0)
  {
    const uint8_t *prev_p = beg;
    for (const uint8_t *p = prev_p; (p = (uint8_t*)memchr(p, '\n', readed + (prev_p - p)) ); ++p)
    {
      if (p) {
        size_t offset = p-prev_p;
        //printf("prev_p=%p p=%p offset=%lu\n", prev_p, p, offset);
        lseek(fd, -(readed-offset), SEEK_CUR);
        strings.emplace_back(prev_p, prev_p+offset);
      }
      prev_p = p+1;
    }

    beg += readed;
  }
  
  return strings;
}

std::vector<uint8_t> File::read_bytes(int max_size)
{
  uint8_t buf[max_size];
  size_t readed = read(fd, &buf, max_size);

  std::vector<uint8_t> data(buf, buf+readed);
  return data;
}

std::vector<uint8_t> File::read_all_bytes()
{
  rewind();
  assert(lseek(fd, 0, SEEK_CUR) == 0);
  obtain_size();
  std::vector<uint8_t> data(this->size);
  
  auto *ptr = data.data();
  std::unique_ptr<uint8_t[]> buf(new uint8_t[FILE_BUF_SIZE]);
  size_t readed = 0;
  size_t saved = 0;
  while ((readed = read(fd, buf.get(), FILE_BUF_SIZE)) > 0)
  {
    std::memcpy(&ptr[0], buf.get(), readed);
    ptr += readed;
    saved += readed;
  }
  
  assert(saved == this->size);
  return data;
}
