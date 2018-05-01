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
#include <assert.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define Z_COMPONENTS 3

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

template <class T>
struct zVector2
{
  T x;
  T y;
};

template <class T>
struct zVector3
{
  T x;
  T y;
  T z;
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
};

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

void zDrawPixel(uint16_t x, uint16_t y, zPixel c);

void zFree();

int zKey(uint32_t key);

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

int64_t zLoadImage(const char *name);
void zDrawImage(const char *name, int32_t x, int32_t y);
int64_t zCreateImage(int w, int h, zPixel col);
void zDrawImage(int64_t imgN, int32_t x, int32_t y);
void zDrawImage(int64_t imgN, int32_t x, int32_t y, double x_scale, double y_scale);

Zimage_t zGetImage(int64_t imgN);

