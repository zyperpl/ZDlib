#include <cstdio>

extern int image_test_main(int, char**);
extern int model_test_main(int, char**);
extern int file_test_main(int, char**);

#define DUMMY(a,b) 0

// tests definitions
#define FILE_TEST(a,b) file_test_main(a,b)
#define IMAGE_TEST(a,b) image_test_main(a,b)
#define MODEL_TEST(a,b) model_test_main(a,b)

#ifndef IMAGE_TEST
  #define IMAGE_TEST(a,b) DUMMY(a,b)
#endif

#ifndef MODEL_TEST
  #define MODEL_TEST(a,b) DUMMY(a,b)
#endif

#ifndef FILE_TEST
  #define FILE_TEST(a,b) DUMMY(a,b)
#endif

auto main(int argc, char *argv[])->int  
{
  if (FILE_TEST(argc, argv) > 0) {
    puts("File test ERROR");
    return 3;
  }

  if (IMAGE_TEST(argc, argv) > 0) {
    puts("Image test ERROR");
    return 1;
  }

  if (MODEL_TEST(argc, argv) > 0) {
    puts("Model test ERROR");
    return 2;
  }



  return 0;
}
