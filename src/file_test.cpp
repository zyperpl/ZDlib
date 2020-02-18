#include <cassert>
#include <cstring>

#include "ZD/File.hpp"

int file_test_main(int, char**)
{
  {
    File f("test", File::Read);
    assert(!f.is_open());
    assert(f.get_size() == 0);
  }

  {
    File f("images/Crate1.mtl", File::Read);
    assert(f.is_open());
    if (f.is_open()) {
      printf("File '%s' opened.\n", f.get_name().data());
    }
    assert(f.get_size() == 272);
    assert(f.get_name() == "images/Crate1.mtl");

    auto bytes = f.read_bytes(10);
    assert(strncmp((char*)bytes.data(), "# Blender ", 10) == 0);
    assert(bytes[0] == '#'); assert(bytes[9] == ' ');

    auto all = f.read_all_bytes();
    assert(all.size() == 272);
    assert(all.size() == f.get_size());
    assert(strncmp((char*)all.data(), "# Ble", 5) == 0);
    assert(all[270] == 'g');
    assert(all[271] == '\n');
  }

  {
    printf("Reading file... ");
    File f("images/Crate1.mtl", File::Read);
    assert(f.is_open() && f.get_size() <= 272);
    
    auto b = f.read_bytes(370);
    assert(b.size() == f.get_size());
    assert(b.size() == 272);
    assert(f.read_all_bytes().size() == 272);
    if (b.size() == 272 && f.read_all_bytes().size() == b.size()) {
      printf("DONE\n");
    } else {
      printf("ERROR!\n");
      assert(false);
    }
  }

  {
    printf("Reading line by line... ");
    File f("images/Crate1.mtl", File::Read);
    assert(f.is_open() && f.get_size() > 0);
    auto line = f.read_line();
    assert(line != std::nullopt);
    f.rewind();
    auto l2 = f.read_line();
    assert(line == l2);
    //printf("line=%s\n", line->data());
    assert(strcmp((*line).c_str(), "# Blender MTL File: 'Crate1.blend'") == 0);

    f.rewind();
    int lines = 0;
    while(auto l = f.read_line()) {
      lines++;
    }
    printf("Read %d lines.\n", lines);
    assert(lines == 12);

    if (line) {
      printf("DONE\n");
    } else {
      printf("ERROR!\n");
      assert(false);
    }
  }
  
  {
    printf("Reading all lines... ");
    File f("images/Crate1.mtl", File::Read);
    assert(f.is_open() && f.get_size() > 0);
    auto lines = f.read_lines();
    printf("Read %lu. lines.\n", lines.size());
    size_t lines_num = 0;
    for (const auto &l: lines) {
      lines_num++;
      //printf("line = '%s'\n", l.data());
      (void)(l);
    }
    assert(lines_num == 12);
    assert(lines.size() == lines_num);
    if (lines.size() == 12) {
      printf("DONE\n");
    } else {
      printf("ERROR!\n");
      assert(false);
    }
  }

  {
    //File f("images/test.txt", File::Write);
  }

  return 0;
}
