# ZDLib (version 1.9)
2D pixel drawing and 3D model rendering library. Supports drawing 2D pixel graphics and textured 3D models with GLSL shaders to the window (or TTY framebuffer).
Library is in early stage of development.

## Dependencies
(for X11 rendering)
- The OpenGL Extension Wrangler Library ([GLEW](http://glew.sourceforge.net/))
- An OpenGL helper library ([GLFW](http://www.glfw.org/))

### Compilation
To compile it under Linux link these libraries: lm lGL lGLU lpthread lglfw lGLEW. See CMakeLists.txt.

### License
Distributed under GNU General Public License v3.0. See: [LICENSE](LICENSE) file
