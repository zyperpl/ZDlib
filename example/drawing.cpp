#include "zdlib.h"

int main()
{
  auto background = zLoadImage("background.png");

  const int IMAGE_SIZE = 100;
  auto img = zCreateImage(IMAGE_SIZE, IMAGE_SIZE);

  for (int i = 0; i < IMAGE_SIZE/2; i++)
  {
    zDrawPixel(img, rand()%IMAGE_SIZE, rand()%IMAGE_SIZE, {30,0,0});
    zDrawPixel(img, i, i, {255,0,0});
  }
  zDrawLine(img, IMAGE_SIZE*10, 0, 10, 30, { 0, 255, 0 });
 
  zDrawImage(background, 10, 10, img);

  const int WINDOW_WIDTH = 800;
  const int WINDOW_HEIGHT = 800;
  auto *wnd = zCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "test_wnd");
  zDrawPixel(wnd, 10, 10, { 255, 255, 0 });


  while (zIsOpen())
  {
    zUpdate();

    if (zKey(ZKEY_QUIT)) zClose();

    zClear(0);
    zDrawPixel(wnd, rand()%WINDOW_WIDTH, rand()%WINDOW_HEIGHT, { (uint8_t)(rand()%100), 100, 130 });

    auto mouse = zMousePosition();
    zDrawLine(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, mouse.x, mouse.y, { 0, 0, 255 });

    zDrawImage(30, 30, background);
    zDrawImage(400, 200, "lena.png", 2, 3);

    zRender();
  }
  

  zSaveImage(background, "test_img.png");
}
