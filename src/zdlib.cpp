#include "zdlib.h"

#ifndef STBI_MALLOC
#define STBI_MALLOC(sz)           malloc(sz)
#define STBI_REALLOC(p,newsz)     realloc(p,newsz)
#define STBI_FREE(p)              free(p)
#endif


#include "stb_image.h"

static Zwindow_t *Zwindow = NULL;

static Zimage_t *Zimages = NULL;
static uint32_t ZimagesNumber = 0;

/*
 * window
 */


// shaders
const GLchar* vertexSource = R"glsl(
    #ifdef GL_ES
    precision highp float;
    #endif
    attribute vec2 position;
    attribute vec2 texcoord;
    varying vec2 uv;
    void main()
    {
        uv = texcoord;
        gl_Position = vec4(position, 0.0, 0.5);
    }
)glsl";
const GLchar* fragmentSource = R"glsl(
    #ifdef GL_ES
    precision highp float;
    #endif
    varying vec2 uv;
    uniform sampler2D tex;
    void main()
    {
        gl_FragColor = texture2D(tex, uv);
    }
)glsl";

zPixel zRGB (uint8_t r, uint8_t g, uint8_t b)
{
  return { r, g, b, 255 };
}

zPixel zRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  return { r, g, b, a };
}

Zwindow_t *zCreateWindow(uint32_t w, uint32_t h, const char *name, uint8_t scale)
{
  glfwInit();

  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  

  // initialize Zwindow
  Zwindow = new Zwindow_t();
  Zwindow->width = w;
  Zwindow->height = h;
  Zwindow->aspect = (double)(w)/(double)(h);
  Zwindow->name = name;
  Zwindow->buffer = new uint8_t[w*h*Z_COMPONENTS];

  // create window
  GLFWwindow *wnd = glfwCreateWindow(w*scale, h*scale, name, NULL, NULL);
  glfwMakeContextCurrent(wnd);

  glewExperimental = GL_TRUE; 
  glewInit();
  glGetError(); //reset error from glewInit
  glewGetExtension("GL_ARB_gpu_shader5");


  // create vertices
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);

  GLfloat vertices[] = {
      -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
       0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
       0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
  };

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint ebo;
  glGenBuffers(1, &ebo);

  GLuint elements[] = {
      0, 1, 2,
      2, 3, 0
  };

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  // create shaders
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);

  int logLength;
  char *log;
  log = new char[256]; glGetShaderInfoLog(vertexShader, 512, &logLength, log); if (logLength > 0) printf("Vertex shader error:\n%s\n", log); 
  delete[] log;

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);
  log = new char[256]; glGetShaderInfoLog(fragmentShader, 512, &logLength, log); if (logLength > 0) printf("Fragment shader error:\n%s\n", log); 
  delete[] log;

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  log = new char[256]; glGetProgramInfoLog(shaderProgram, 512, &logLength, log); if (logLength > 0) printf("Shader program error:\n%s\n", log); 
  delete[] log;  

  glUseProgram(shaderProgram);
  
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

  GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

  // create texture
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  memset(&Zwindow->buffer[0], 0, w*h*Z_COMPONENTS);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, 
               GL_RGB, GL_UNSIGNED_BYTE, &Zwindow->buffer[0]);
    
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  return Zwindow;
}

void zSetWindow(Zwindow_t *wnd)
{
  Zwindow = wnd;
}

Zwindow_t *zGetWindow()
{
  return Zwindow;
}

bool zIsOpen()
{
  return !glfwWindowShouldClose(glfwGetCurrentContext());
}

void zClear()
{
  if (Zwindow) memset(&Zwindow->buffer[0], 0, Zwindow->width*Zwindow->height*Z_COMPONENTS);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void zUpdate()
{
  // update texture
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Zwindow->width, Zwindow->height, 0, 
               GL_RGB, GL_UNSIGNED_BYTE, &Zwindow->buffer[0]);  

  int width, height;
  glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

  double r = double(width)/double(Zwindow->width);
  double r2 = double(height)/double(Zwindow->height);
  if (r > r2) r = r2;
  int w = int(r*Zwindow->width);
  int h = int(r*Zwindow->height);
  glViewport((width-w)/2, (height-h)/2, w, h);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glfwPollEvents();
  glfwSwapBuffers(glfwGetCurrentContext());
}

void zDrawPixel(uint16_t x, uint16_t y, zPixel c)
{
  if (x >= Zwindow->width) return;
  if (y >= Zwindow->height) return;

  Zwindow->buffer[y*Z_COMPONENTS*Zwindow->width + x*Z_COMPONENTS + 0] = c.r;
  Zwindow->buffer[y*Z_COMPONENTS*Zwindow->width + x*Z_COMPONENTS + 1] = c.g;
  Zwindow->buffer[y*Z_COMPONENTS*Zwindow->width + x*Z_COMPONENTS + 2] = c.b;  
}

void zFree()
{
  for (uint32_t i = 0; i < ZimagesNumber; i++)
  {
    stbi_image_free(Zimages[i].data);
  }

  if (Zwindow != NULL) 
  {
    delete[] Zwindow->buffer;
    delete Zwindow;
  }
   
  glfwDestroyWindow(glfwGetCurrentContext());
  glfwTerminate();
}

int zKey(uint32_t key)
{
  return glfwGetKey(glfwGetCurrentContext(), key);
}

zVector2<double> zGetMousePosition()
{
  zVector2<double> pos;
  glfwGetCursorPos(glfwGetCurrentContext(), &pos.x, &pos.y);
  return pos;
}

int zMouseButton(uint32_t btn)
{
  return glfwGetMouseButton(glfwGetCurrentContext(), btn);
}

/*
 * images
 */

int64_t zLoadImage(const char *name)
{
  int64_t foundAt = -1;
  for (uint32_t i = 0; i < ZimagesNumber && foundAt < 0; i++)
  {
    if (strcmp(Zimages[i].name, name) == 0) 
    {
      foundAt = i;
    }

  }
  // already exists
  if (foundAt >= 0) return foundAt;

  // create new image
  Zimage_t *imgs = new Zimage_t[ZimagesNumber+1];
  if (Zimages) memcpy(imgs, Zimages, sizeof(Zimage_t) * ZimagesNumber);
  imgs[ZimagesNumber].name = name;

  imgs[ZimagesNumber].data = stbi_load(name, 
      &imgs[ZimagesNumber].w, &imgs[ZimagesNumber].h, 
      &imgs[ZimagesNumber].components, 0);

  if (!imgs[ZimagesNumber].data)
  {
    // data not loaded
    delete[] imgs;
    return -1;
  }

  delete[] Zimages;
  Zimages = imgs;

  ZimagesNumber++;

  return ZimagesNumber-1;
}

void zDrawImage(const char *name, int32_t x, int32_t y)
{
  int64_t foundAt = -1;
  for (uint32_t i = 0; i < ZimagesNumber && foundAt < 0; i++)
  {
    if (Zimages[i].name == name) foundAt = i;
  }
  if (foundAt < 0)
  {
    foundAt = zLoadImage(name);
    assert(foundAt >= 0);
  }

  zDrawImage(foundAt, x, y);  
}

void zDrawImage(int64_t imgN, int32_t x, int32_t y)
{
  zDrawImage(imgN, x, y, 1, 1);  
}

void zDrawImage(int64_t imgN, int32_t x, int32_t y, double x_scale, double y_scale)
{
  assert(imgN >= 0 && imgN < ZimagesNumber);

  //TODO implement y_scale
  (void)y_scale;

  Zimage_t *img = &Zimages[imgN];

  if (x+img->w < 0)                   return;
  if (x >= (int32_t)Zwindow->width)   return;
  if (y >= (int32_t)Zwindow->height)  return;
  if (y+img->h < 0)                   return;

  int32_t i = 0, p;
  for (i = 0, p = (x + (x_scale < 0 ? img->w : 0))*Z_COMPONENTS + y*Z_COMPONENTS*Zwindow->width; 
      i < img->w * img->h * img->components && p < (int32_t)(Zwindow->width*Zwindow->height*Z_COMPONENTS);
      i += img->components, p += Z_COMPONENTS*((int)x_scale*10)/10)
  {
   
    if (i != 0 && i/img->components % img->w == 0) p += (Zwindow->width-(img->w*((int)x_scale)))*Z_COMPONENTS;

    if ((i/img->components)%img->w + x < (int32_t)Zwindow->width
    &&  (i/img->components)%img->w + x >= 0) 
    {
      if (p > 0 && img->data[i+3] != 0)
      {
        Zwindow->buffer[p+0] = img->data[i+0];
        Zwindow->buffer[p+1] = img->data[i+1];
        Zwindow->buffer[p+2] = img->data[i+2];    
      }
    }
  }
  
  // update texture
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Zwindow->width, Zwindow->height, 0, 
               GL_RGB, GL_UNSIGNED_BYTE, &Zwindow->buffer[0]);  
}

Zimage_t zGetImage(int64_t imgN)
{
  assert(imgN >= 0 && imgN < ZimagesNumber);

  return Zimages[imgN];
}

int64_t zCreateImage(int w, int h, zPixel col)
{
  const int comp = Z_COMPONENTS;

  //search
  int64_t foundAt = -1;
  for (uint32_t i = 0; i < ZimagesNumber && foundAt < 0; i++)
  {
    if (strcmp(Zimages[i].name, "_z_custom") == 0
    &&  Zimages[i].w > 0
    &&  Zimages[i].w == w
    &&  Zimages[i].h == h
    &&  Zimages[i].components == comp
    &&  Zimages[i].data[0] == col.r
    &&  Zimages[i].data[1] == col.g
    &&  Zimages[i].data[2] == col.b) 
    {
      foundAt = i;
    }

  }
  // already exists
  if (foundAt >= 0) return foundAt;


  //create new
  Zimage_t *imgs = new Zimage_t[ZimagesNumber+1];
  if (!imgs) return -1;
  if (Zimages) memcpy(imgs, Zimages, sizeof(Zimage_t) * ZimagesNumber);
  imgs[ZimagesNumber].name = "_z_custom";

  imgs[ZimagesNumber].w = w;
  imgs[ZimagesNumber].h = h;
  imgs[ZimagesNumber].components = comp;

  imgs[ZimagesNumber].data = (uint8_t*)STBI_MALLOC(w*h*comp);

   if (!imgs[ZimagesNumber].data)
  {
    // data not alloc'd
    delete[] imgs;
    return -1;
  }

  memset(imgs[ZimagesNumber].data, 120, w*h*comp);

  for (int64_t i = 0; i <= w*h*comp; i += comp)
  {
    imgs[ZimagesNumber].data[i+0] = col.r;
    imgs[ZimagesNumber].data[i+1] = col.g;
    imgs[ZimagesNumber].data[i+2] = col.b;
  }


  delete[] Zimages;
  Zimages = imgs;

  ZimagesNumber++;

  return ZimagesNumber-1;

}
