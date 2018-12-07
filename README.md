# ZDLib
2D pixel drawing **single-file** library. Library creates window with **OpenGL3 context** and draws into OpenGL texture or can draw directly into TTY framebuffer. 
 Library is in **very early** development stage and should be used for simple applications.

## Dependencies
(for X11 rendering)
- The OpenGL Extension Wrangler Library ([GLEW](http://glew.sourceforge.net/))
- An OpenGL helper library ([GLFW](http://www.glfw.org/))

### Compilation
To compile it under Linux:
```bash
g++ main.cpp -I $PATH_TO_LIB/src $PATH_TO_LIB/src/zdlib.cpp $PATH_TO_LIB/src/stb_image.cpp -lm -lGL -lGLU -lpthread -lglfw -lGLEW 
```
See Example below for more details or view [example makefile](example/makefile).
To compile it without X11 support use `-DNO_X11=1`.

### Usage
To use library include zdlib.h header: ```#include "zdlib.h"```
Then you can use following functions:
```cpp
Zwindow_t *zCreateWindow(uint32_t w, uint32_t h, const char *name, uint8_t scale = 1);
```
Creates window with OpenGL context and shows it on the screen or allocates framebuffer for TTY. 
Parameters ```w``` and ```h``` sets screen buffer size, and ```scale``` sets multiplicity of screen buffer size as window size.
```name``` sets window title.
It is possible to create multiple windows and change windows with commands:
```cpp 
void zSetWindow(Zwindow_t *wnd);
Zwindow_t *zGetWindow();
```
<br/>

```c
bool zIsOpen();
```
Checks if current window is open.

```c
void zClose();
```
Closes current window.

```c
void zClear();
```
Clears the buffer.

```c
void zUpdate();
```
Swap current buffer and polls current window events.

```c
void zDrawPixel(uint16_t x, uint16_t y, zPixel color);
```
Draws pixel at given location with given ```color```. ```zPixel``` is a structure that contains 4 color components (RGBA):
```c
struct zPixel
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a = 255;
};
```
You can use this function like this: ```zDrawPixel(point.x, point.y, { 255, 0, 0 });``` to draw red point.

```cpp
void zFree();
```
**Must be called** at the end of the program. Frees all resources and closes the window.

<br/>

#### Input functions
```cpp
int zKey(uint32_t key);

zVector2<double> zGetMousePosition();
int zMouseButton(uint32_t btn);
```
Use predefined ```ZKEY_*``` aliases or ```GLFW_KEY_*``` as ```key``` and ```ZMOUSE_BUTTON_*``` or ```GLFW_MOUSE_BUTTON_*``` as  mouse ```btn``.


#### Image load functions
Library supports image loading via [stb_image](https://github.com/nothings/stb). All images are remembered in the pool and are freed automatically when ```zFree``` is called.
```cpp
int64_t zLoadImage(const char *name);
void zDrawImage(const char *name, int32_t x, int32_t y);
int64_t zCreateImage(int w, int h, zPixel col);
void zDrawImage(int64_t imgN, int32_t x, int32_t y);
void zDrawImage(int64_t imgN, int32_t x, int32_t y, double x_scale, double y_scale);

Zimage_t zGetImage(int64_t imgN);
```

### Example
Example is simple multithreaded demo that reads image and shows random pixels on the screen. It also shows how to read user mouse input.

<br/>

![gif1](example/zdlib1.gif) ![gif2](example/zdlib3.gif)

### License
Distributed under GNU General Public License v3.0. See: [LICENSE](LICENSE) file
