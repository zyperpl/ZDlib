/*
 *  ZDLib library created by Kacper "Zyper" Zybała (2017)
 *
 *  Licensed under MIT License
 *  
MIT License

Copyright (c) 2017 Kacper Zybała

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cmath>
#include <assert.h>

//#define NO_X11
#if NO_X11 == 0
  #undef NO_X11
#endif

#define Z_COMPONENTS 4

#ifndef NO_X11
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// TODO: add library independent key names
#define ZKEY_LEFT    GLFW_KEY_LEFT
#define ZKEY_RIGHT   GLFW_KEY_RIGHT
#define ZKEY_UP      GLFW_KEY_UP
#define ZKEY_DOWN    GLFW_KEY_DOWN
#define ZKEY_ACTION  GLFW_KEY_SPACE
#define ZKEY_QUIT    GLFW_KEY_ESCAPE

#define ZMOUSE_BUTTON_LEFT    GLFW_MOUSE_BUTTON_LEFT
#define ZMOUSE_BUTTON_RIGHT   GLFW_MOUSE_BUTTON_RIGHT
#define ZMOUSE_BUTTON_MIDDLE  GLFW_MOUSE_BUTTON_MIDDLE

#else

#undef Z_COMPONENTS
#define Z_COMPONENTS 4

#include "/usr/include/linux/input-event-codes.h"
#define ZKEY_LEFT    KEY_LEFT
#define ZKEY_RIGHT   KEY_RIGHT
#define ZKEY_UP      KEY_UP
#define ZKEY_DOWN    KEY_DOWN
#define ZKEY_ACTION  KEY_SPACE
#define ZKEY_QUIT    KEY_ESC

#define ZMOUSE_BUTTON_LEFT    0
#define ZMOUSE_BUTTON_RIGHT   2
#define ZMOUSE_BUTTON_MIDDLE  1

#define FB_FILE   "/dev/fb0"
#define FB_WIDTH  1920
#define FB_HEIGHT 1080

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define KB_FILE   "/dev/input/event9"
#include "/usr/include/linux/input.h"

#endif

#define ZAUDIO

#ifdef ZAUDIO
  #include <portaudio.h>
  
  #define ZVOICES_N     128
  #define ZSAMPLE_RATE  44100
  #define ZFRAMES       1024

  typedef struct
  {
    float frequency;
    float volume{0.1};
    bool enabled{false};
    long phase{0};
  } Zvoice;

  typedef struct
  {
    Zvoice voice[ZVOICES_N];
  } ZaudioData;

#endif

template <class T>
struct zVector2
{
  T x;
  T y;

  zVector2() {}
  zVector2(T xc, T yc) : x{xc}, y{yc} {};
  const zVector2<T> operator+(const zVector2<T> ov) { return { x + ov.x, y + ov.y }; }
  const zVector2<T> operator-(const zVector2<T> ov) { return { x - ov.x, y - ov.y }; }
  double distancePow(const zVector2<T> ov) { return pow(ov.x-x,2) + pow(ov.y-y,2); }
  const zVector2<T> operator/=(const T v) { x=x/v; y=y/v; return *this; }
  bool operator!=(const zVector2<T> ov) { return ov.x!=x || ov.y!=y; }
  zVector2<T> operator*(const float &f) { return { x*f, y*f }; }
};

template <class T>
struct zVector3
{
  T x;
  T y;
  T z;

  zVector3() {}
  zVector3(T xc, T yc, T zc) : x{xc}, y{yc}, z{zc} {};
};

/*
 * pixels
 */
struct zPixel
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a = 255;

  zPixel() {}
  zPixel(uint8_t rc, uint8_t gc, uint8_t bc, uint8_t ac = 255) : r{rc}, g{gc}, b{bc}, a{ac} {}

  zPixel operator*(const uint8_t &v) { 
    return { static_cast<uint8_t>(r*v), static_cast<uint8_t>(g*v), static_cast<uint8_t>(b*v) }; 
  }
  
  zPixel operator*(const double &v) { 
    return { static_cast<uint8_t>((double)r*v), static_cast<uint8_t>((double)g*v), static_cast<uint8_t>((double)b*v) }; 
  }
  
  zPixel operator/(const double &v) { 
    return { static_cast<uint8_t>((double)r/v), static_cast<uint8_t>((double)g/v), static_cast<uint8_t>((double)b/v) }; 
  }

  zPixel operator+(const zPixel &op) {
    int nr = r+op.r; if (nr > 255) nr = 255;
    int ng = g+op.g; if (ng > 255) ng = 255;
    int nb = b+op.b; if (nb > 255) nb = 255;

    return { static_cast<uint8_t>(nr), static_cast<uint8_t>(ng), static_cast<uint8_t>(nb) };
  }
  
  zPixel operator-=(const int &v) { 
    int nr = r-v; if (nr < 0) nr = 0;
    int ng = g-v; if (ng < 0) ng = 0;
    int nb = b-v; if (nb < 0) nb = 0;
    r = nr; g = ng; b = nb;
    return *this;
  }
  
  zPixel operator-=(const zPixel &op) { 
    int nr = r-op.r; if (nr < 0) nr = 0;
    int ng = g-op.g; if (ng < 0) ng = 0;
    int nb = b-op.b; if (nb < 0) nb = 0;
    r = nr; g = ng; b = nb;
    return *this;
  }

  zPixel operator-(const zPixel &op) {
    int nr = r-op.r; if (nr < 0) nr = 0;
    int ng = g-op.g; if (ng < 0) ng = 0;
    int nb = b-op.b; if (nb < 0) nb = 0;
    return zPixel(nr,ng,nb);
  };

  bool operator==(const zPixel &op) {
    return r == op.r && g == op.g && b == op.b;
  }
  
  bool operator!=(const zPixel &op) {
    return r != op.r || g != op.g || b != op.b;
  }
};

zPixel zRGB(uint8_t c);
zPixel zRGB (uint8_t r, uint8_t g, uint8_t b);
zPixel zRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/*
 * window
 */
typedef struct
{
  uint32_t width;
  uint32_t height;
  double aspect;  
  const char *name;

  uint8_t *buffer;
} Zwindow_t;

Zwindow_t *zCreateWindow(uint32_t w, uint32_t h, const char *name, uint8_t scale = 1);

void zSetWindow(Zwindow_t *wnd);
Zwindow_t *zGetWindow();

bool zIsOpen();

void zClose();

void zClear();

void zUpdate();
void zRender();


void zDrawPixel(uint16_t x, uint16_t y, zPixel c);
void zDrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, zPixel c);

void zFree();

int zKey(uint32_t key);
uint32_t zLastCharacter();

zVector2<double> zGetMousePosition();
int zMouseButton(uint32_t btn);


/*
 * texture
 */
typedef struct
{
  const char *name;
  uint8_t *data;
  int w, h, components;
} Zimage_t;

typedef int64_t zimg;

zimg zLoadImage(const char *name);
void zDrawImage(const char *name, int32_t x, int32_t y);
zimg zCreateImage(int w, int h, zPixel col, int comp = Z_COMPONENTS);
zimg zCreateImage(int w, int h, int comp = Z_COMPONENTS);
void zDrawImage(zimg imgN, int32_t x, int32_t y);
void zDrawImage(zimg imgN, int32_t x, int32_t y, double x_scale, double y_scale);

bool zSaveImage(zimg img, const char *fileName);

zPixel zGetPixel(uint16_t x, uint16_t y);
zPixel zGetPixel(double x, double y);
zPixel zGetImagePixel(zimg img, uint16_t x, uint16_t y);

void zSetImagePixel(zimg imgN, int32_t x, int32_t y, zPixel col);
void zImageDrawImage(zimg target, zimg source, int32_t x, int32_t y);
void zClearImage(zimg img);

Zimage_t zGetImage(zimg imgN);

int zInitAudio(int channels = 1, long sampleRate = ZSAMPLE_RATE, long frames = ZFRAMES);
Zvoice &zVoice(size_t id);
float zAudioOutput(size_t idx);
