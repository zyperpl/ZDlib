#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <cmath>

#include "zdlib.h"

#define UPDATE_INTERVAL_MS 30

#define W 500
#define H 500

void updateWater(zimg imgN, int waterX, int waterY)
{
  auto image = zGetImage(imgN);
  auto window = *zGetWindow();
  auto buf = window.buffer;
  auto bufW = window.width;
  auto bufH = window.height;
  auto bufComp = Z_COMPONENTS;

  auto getPixel = [](uint8_t *buf, int idx) -> zPixel
  {
    return { buf[idx+0], buf[idx+1], buf[idx+2] };
  };

  auto getSourcePixel = [buf, bufW, bufH, bufComp, getPixel](double x, double y) -> zPixel
  {
    if (x < 0) return { 0, 0, 0 };
    if (y < 0) return { 0, 0, 0 };
    if (x >= bufW) return { 0, 0, 0 };
    if (y >= bufH) return { 0, 0, 0 };
    
    // interpolation
    return getPixel(buf, ((int)(x) + (int)(y)*bufH)*bufComp);
  };

  for (int y = 0; y < image.h; y++)
  for (int x = 0; x < image.w; x++)
  {
    zVector2<int> wp(waterX+x-sin(x)*2, waterY+y-cos(y)*4);
    auto px = getSourcePixel(wp.x, wp.y);
    if (px.b < 128) px.b *= 2; else px.b = 255;
    
    zDrawPixel(imgN, x, y, px);
  }
}

int main()
{
  zCreateWindow(W,H, "zdlib test - water", 2);

  zDrawImage(W/2, H/2, "lena.png");
  zimg water = zCreateImage(W, H/2);
  
  while (zIsOpen())
  {
    static double t = 0;
    t += 1;

    //draw scene
    zDrawImage(0, 0, "background.png");
    int imgY = (H/2)-(int)(cosf(t/10.0)*100.);
    zDrawImage(W/2, imgY, "lena.png");

    //draw water
    if (!zKey(ZKEY_ACTION))
    {
      int waterX = 0;
      int waterY = H/2;
      updateWater(water, waterX, waterY);
      zDrawImage(waterX, waterY, water);
    };

    // render
    zUpdate();
    zRender();

    char fileName[255];
    sprintf(fileName, "water_effect_%03d.png", (int)(t));
    //zScreenshot(fileName);

    std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL_MS));
  }
};
