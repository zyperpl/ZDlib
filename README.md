# ZDLib
2D pixel drawing and 3D model rendering library. Supports drawing 2D pixel graphics and textured 3D models with GLSL shaders to the window (or TTY framebuffer).
Library provides many features:
- Invoking actions on file change
- Buffer reader/builder helper classes
- Image loader
- 3D Models loader
- Handling user keyboard and mouse input
- Built-in and custom GLSL shader loader
- 3D camera and projection support
- 2D tileset support
- File manipulation helper class
- OpenGL 3.4 Renderer
- Rendering 2D sprites with GLSL shaders
- Support for 2D Animation
- 2D bitmap pixel drawing

## Dependencies
(for X11 rendering)
- The OpenGL Extension Wrangler Library ([GLEW](http://glew.sourceforge.net/))
- An OpenGL helper library ([GLFW](http://www.glfw.org/))

### Compilation
To compile it under Linux link these libraries: lm lGL lGLU lpthread lglfw lGLEW. See CMakeLists.txt.

### License
Distributed under GNU General Public License v3.0. See: [LICENSE](LICENSE) file
