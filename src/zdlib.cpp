#include "zdlib.h"


#ifndef STBI_MALLOC
#define STBI_MALLOC(sz)           malloc(sz)
#define STBI_REALLOC(p,newsz)     realloc(p,newsz)
#define STBI_FREE(p)              free(p)
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

static Zwindow_t *Zwindow = NULL;

static Zimage_t *Zimages = NULL;
static uint32_t ZimagesNumber = 0;

#ifdef NO_X11
static FILE *fb = NULL;
typedef char GLchar;

static int input = 0;
#endif

static int keys[512] = {0};

#define CHAR_BUF_SIZE 8
uint32_t charBuffer[CHAR_BUF_SIZE];
uint32_t charBufferLastIdx = 0;

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

zPixel zRGB (uint8_t c)
{
  return { c, c, c, 255 };
}

zPixel zRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  return { r, g, b, a };
}

Zwindow_t *zCreateWindow(uint32_t w, uint32_t h, const char *name, uint8_t scale)
{
  // initialize Zwindow
  Zwindow = new Zwindow_t();
  Zwindow->width = w;
  Zwindow->height = h;

#ifndef NO_X11
  glfwInit();

  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  
#else
  w = FB_WIDTH;
  h = FB_HEIGHT;
#endif

  Zwindow->aspect = (double)(w)/(double)(h);
  Zwindow->name = name;
  Zwindow->buffer = new uint8_t[w*h*Z_COMPONENTS];

#ifndef NO_X11
  // create window
  GLFWwindow *wnd = glfwCreateWindow(w*scale, h*scale, name, NULL, NULL);
  glfwMakeContextCurrent(wnd);

  // callbacks
  glfwSetCharModsCallback(wnd, [](GLFWwindow* window, unsigned int codepoint, int mods)
  {
    charBufferLastIdx = (charBufferLastIdx+1)%CHAR_BUF_SIZE;
    charBuffer[charBufferLastIdx%CHAR_BUF_SIZE] = codepoint;
  });
  
  glfwSetKeyCallback(wnd, [](GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    keys[key] = action;
  });

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

  //int logLength;
  //char *log;
  //log = new char[256]; glGetShaderInfoLog(vertexShader, 512, &logLength, log); if (logLength > 0) printf("Vertex shader error:\n%s\n", log); delete[] log;

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);
  //log = new char[256]; glGetShaderInfoLog(fragmentShader, 512, &logLength, log); if (logLength > 0) printf("Fragment shader error:\n%s\n", log); delete[] log;

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  //log = new char[256]; glGetProgramInfoLog(shaderProgram, 512, &logLength, log); if (logLength > 0) printf("Shader program error:\n%s\n", log); delete[] log;  

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

  glDisable(GL_DEPTH_BUFFER);
#else
  fb = fopen(FB_FILE, "wb");
  if (!fb) fprintf(stderr, "Cannot read framebuffer %s!\n", FB_FILE);

  //input = fopen(KB_FILE, "rb");
  input = open(KB_FILE, O_NONBLOCK | O_RDONLY);
  if (!input) fprintf(stderr, "Cannot read keyboard input %s!\n", KB_FILE);
#endif
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
#ifndef NO_X11
  return !glfwWindowShouldClose(glfwGetCurrentContext());
#else
  return fb!=NULL;
#endif
}

void zClear()
{
  int w = Zwindow->width;
  int h = Zwindow->height;

#ifdef NO_X11
  w = FB_WIDTH;
  h = FB_HEIGHT;
#endif

  if (Zwindow) memset(&Zwindow->buffer[0], 0, w*h*Z_COMPONENTS);
#ifndef NO_X11
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
}

void zRender()
{
#ifndef NO_X11
  // update texture
  /*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Zwindow->width, Zwindow->height, 0, 
               GL_RGB, GL_UNSIGNED_BYTE, &Zwindow->buffer[0]);  */

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Zwindow->width, Zwindow->height, 
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

#else
  const int SIZE = FB_WIDTH * FB_HEIGHT * Z_COMPONENTS;

  //TODO: RGB to BGR
  #pragma omp parallel for
  for (int i = 0; i < SIZE; i+=Z_COMPONENTS)
  {
    auto t = Zwindow->buffer[i+0];
    Zwindow->buffer[i+0] = Zwindow->buffer[i+2];
    Zwindow->buffer[i+2] = t;
  }
  
  int written = fwrite(&Zwindow->buffer[0], sizeof(uint8_t), SIZE, fb);
  fclose(fb); fopen(FB_FILE, "wb");
  if (written <= 0) 
  {
    fprintf(stderr, "Cannot write into frame buffer!\n");
  } 
  
#endif
}

void zUpdate()
{
#ifndef NO_X11
  glfwPollEvents();
  glfwSwapBuffers(glfwGetCurrentContext());
#else
  size_t size = sizeof(input_event);
  char bufkb[size];
  int rkb = read(input, bufkb, size);
  if (rkb > 0)
  {
    input_event *iev = reinterpret_cast<input_event*>(bufkb);
    keys[iev->code] = iev->value;
  }
#endif
}

bool zSaveImage(zimg img, const char *fileName)
{
  auto image = zGetImage(img);

  return stbi_write_png(fileName, image.w, image.h, image.components, 
                        image.data, image.w*image.components);
}

void zDrawPixel(uint16_t x, uint16_t y, zPixel c)
{
  if (x >= Zwindow->width) return;
  if (y >= Zwindow->height) return;

  int w = Zwindow->width;
  int h = Zwindow->height;

#ifdef NO_X11
  w = FB_WIDTH;
  h = FB_HEIGHT;
#endif

  long long p = (x+y*w)*Z_COMPONENTS;
  Zwindow->buffer[p + 0] = c.r;
  Zwindow->buffer[p + 1] = c.g;
  Zwindow->buffer[p + 2] = c.b;  
}


void zDrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, zPixel c)
{
  int w = Zwindow->width;
  int h = Zwindow->height;

#ifdef NO_X11
  w = FB_WIDTH;
  h = FB_HEIGHT;
#endif

  auto putPx = [w](int32_t x, int32_t y, zPixel c)
  {
    if (x < 0) return;
    if (y < 0) return;
    if (x >= Zwindow->width) return;
    if (y >= Zwindow->height) return;

    long long p = (x+y*w)*Z_COMPONENTS;
    Zwindow->buffer[p + 0] = c.r;
    Zwindow->buffer[p + 1] = c.g;
    Zwindow->buffer[p + 2] = c.b;  
  };

	bool yLonger=false;
	int incrementVal;

	int shortLen=y2-y1;
	int longLen=x2-x1;
	if (abs(shortLen)>abs(longLen)) {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;
		yLonger=true;
	}

	if (longLen<0) incrementVal=-1;
	else incrementVal=1;

	double divDiff;
	if (shortLen==0) divDiff = longLen;
	else divDiff = (double)longLen / (double)shortLen;
	if (yLonger) 
  {
		for (int i=0; i != longLen; i += incrementVal) 
    {
			putPx(x1+(int)((double)i/divDiff),y1+i, c);
		}
	} else 
  {
		for (int i=0; i != longLen; i+=incrementVal) 
    {
			putPx(x1+i,y1+(int)((double)i/divDiff), c);
		}
	}
}

zPixel zGetPixel(uint16_t x, uint16_t y)
{
  if (x < 0 || y < 0) return zRGB(0);
  if (x >= Zwindow->width) return zRGB(0);
  if (y >= Zwindow->height) return zRGB(0);
  
  int w = Zwindow->width;
  int h = Zwindow->height;

#ifdef NO_X11
  w = FB_WIDTH;
  h = FB_HEIGHT;
#endif
  
  zPixel ret;
  const int idx = (x+y*w)*Z_COMPONENTS;
  ret.r = Zwindow->buffer[idx+0];
  ret.g = Zwindow->buffer[idx+1];
  ret.b = Zwindow->buffer[idx+2];
  if (Z_COMPONENTS > 3) ret.a = Zwindow->buffer[idx+3];
  return ret;
}

zPixel zGetPixel(double x, double y)
{
  // linear interpolation
  auto leftUp = zGetPixel((uint16_t)x, (uint16_t)y);
  auto rightUp = zGetPixel((uint16_t)(x+.5), (uint16_t)(y));
  auto leftDown = zGetPixel((uint16_t)x, (uint16_t)(y+0.5));
  auto rightDown = zGetPixel((uint16_t)(x+.5), (uint16_t)(y+0.5));

  double ratioX = x-(int)(x);
  double ratioY = y-(int)(y);
  
  auto up = leftUp*(1.0-ratioX) + rightUp*ratioX;
  auto down = leftDown*(1.0-ratioX) + rightDown*ratioX;

  auto ret = up*(1.0-ratioY) + down*ratioY;

  return ret;
}

zPixel zGetImagePixel(zimg img, uint16_t x, uint16_t y)
{
  if (x < 0 || y < 0) return zRGB(0);

  auto image = zGetImage(img);
  if (x >= image.w || y >= image.h) return zRGB(0);
  
  uint64_t p = (x+y*image.w)*image.components;
  zPixel ret{
    image.data[p+0],
    image.components > 1 ? image.data[p+1] : image.data[p+0],
    image.components > 2 ? image.data[p+2] : image.data[p+0],
    image.components > 3 ? image.data[p+3] : image.data[p+0]
  };
  return ret;
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

#ifndef NO_X11
  glfwDestroyWindow(glfwGetCurrentContext());
  glfwTerminate();
#else
  fclose(fb);
  close(input);
#endif
}

int zKey(uint32_t key)
{
  //return glfwGetKey(glfwGetCurrentContext(), key);
  return keys[key];  
}

uint32_t zLastCharacter()
{
  auto ch = charBuffer[(charBufferLastIdx)%CHAR_BUF_SIZE];
  charBuffer[(charBufferLastIdx)%CHAR_BUF_SIZE] = 0;
  if (ch > 0)
  {
    charBufferLastIdx--; 
  }
  return ch;
}

zVector2<double> zGetMousePosition()
{
  zVector2<double> pos;
#ifndef NO_X11
  glfwGetCursorPos(glfwGetCurrentContext(), &pos.x, &pos.y);

  int width, height;
  glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

  double r = double(width)/double(Zwindow->width);
  double r2 = double(height)/double(Zwindow->height);
  if (r > r2) r = r2;
  int w = int(r*Zwindow->width);
  int h = int(r*Zwindow->height);
  
  pos.x -= (width-w)/2;
  pos.y -= (height-h)/2;
  pos.x /= r;
  pos.y /= r;

#endif
  return pos;
}

int zMouseButton(uint32_t btn)
{
#ifndef NO_X11
  return glfwGetMouseButton(glfwGetCurrentContext(), btn);
#endif
  return 0;
}

/*
 * images
 */

zimg zLoadImage(const char *name)
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
    fprintf(stderr, "Image %s cannot be loaded!\n", name);
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

void zDrawImage(zimg imgN, int32_t x, int32_t y)
{
  zDrawImage(imgN, x, y, 1, 1);  
}

void zDrawImage(zimg imgN, int32_t x, int32_t y, double x_scale, double y_scale)
{
  assert(imgN >= 0 && imgN < ZimagesNumber);

  //TODO implement y_scale
  (void)y_scale;

  Zimage_t *img = &Zimages[imgN];
  
  //printf("%s: components=%d [0,0]=%d;%d;%d;%d\n", img->name, img->components, img->data[0+0], img->data[0+1], img->data[0+2], img->data[0+3]);

  if (x+img->w < 0)                   return;
  if (x >= (int32_t)Zwindow->width)   return;
  if (y >= (int32_t)Zwindow->height)  return;
  if (y+img->h < 0)                   return;
  
  int w = Zwindow->width;
  int h = Zwindow->height;

#ifdef NO_X11
  w = FB_WIDTH;
  h = FB_HEIGHT;
#endif

  auto *buf = Zwindow->buffer;

  if ((x_scale == -1 || x_scale == 1) && y_scale == 1)
  {
    // legacy (probably faster)
    
    int i, p;
    for (i = 0, p = (x + (x_scale < 0 ? img->w : 0))*Z_COMPONENTS + y*Z_COMPONENTS*w; 
        i < img->w * img->h * img->components && p < (int32_t)(w*h*Z_COMPONENTS);
        i += img->components, p += Z_COMPONENTS*((int)x_scale*10)/10)
    {
    
      if (i != 0 && i/img->components % img->w == 0) p += (w-(img->w*((int)x_scale)))*Z_COMPONENTS;

      if ((i/img->components)%img->w + x < (int32_t)w
      &&  (i/img->components)%img->w + x >= 0) 
      {
        if (p > 0)
        {
          //TODO: implement alpha blending
          //TODO: implement y-scale
          if (img->components > 3 && img->data[i+3] == 0) continue;

          Zwindow->buffer[p+0] = img->data[i+0];
          Zwindow->buffer[p+1] = img->components > 1 ? img->data[i+1] : img->data[i+0];
          Zwindow->buffer[p+2] = img->components > 2 ? img->data[i+2] : img->data[i+0];    
        }
      }
    }
  } else
  {
    // for different scales
    
    int niw = img->w*std::abs(x_scale);
    int nih = img->h*std::abs(y_scale);

    #pragma omp parallel for
    for (int i = 0; i < niw*nih; i++)
    {
      int bx = i%niw;
      int by = i/niw;

      if (x+bx >= w) continue;
      if (y+by >= h) continue;

      if (x+bx < 0) continue;
      if (y+by < 0) continue;

      int ix = bx/std::abs(x_scale);
      int iy = by/std::abs(y_scale);

      if (x_scale < 0) ix = img->w-ix;
      if (y_scale < 0) iy = img->h-iy;

      int bufidx = (bx+by*w+(x+y*w))*Z_COMPONENTS;
      int imgidx = (ix+iy*img->w)*img->components;

      buf[bufidx + 0] = img->data[imgidx+0];
      buf[bufidx + 1] = img->data[imgidx+1];
      buf[bufidx + 2] = img->data[imgidx+2];
    }
  }

}

void zImageDrawImage(zimg target, zimg source, int32_t x, int32_t y)
{
  Zimage_t *t = &Zimages[target];
  Zimage_t *s = &Zimages[source];
  
  if (x+s->w < 0)  return;
  if (x >= t->w)   return;
  if (y >= t->h)   return;
  if (y+s->h < 0)  return;

  double x_scale = 1.0, y_scale = 1.0;
  
  int i, p;
  for (i = 0, p = (x + (x_scale < 0 ? s->w : 0))*t->components + y*t->components*t->w; 
      i < s->w * s->h * s->components && p < (int32_t)(t->w*t->h*t->components);
      i += s->components, p += t->components*((int)x_scale*10)/10)
  {
   
    if (i != 0 && i/s->components % s->w == 0) p += (t->w-(s->w*((int)x_scale)))*t->components;

    if ((i/s->components)%s->w + x < (int32_t)t->w
    &&  (i/s->components)%s->w + x >= 0) 
    {
      if (p > 0)
      {
        //TODO: implement alpha blending
        if (s->components > 3 && s->data[i+3] == 0) continue;

        t->data[p+0] = s->data[i+0];
        if (t->components > 1) t->data[p+1] = s->components > 1 ? s->data[i+1] : s->data[i+0];
        if (t->components > 2) t->data[p+2] = s->components > 2 ? s->data[i+2] : s->data[i+0];    
      }
    }
  }

}

Zimage_t zGetImage(zimg imgN)
{
  assert(imgN >= 0 && imgN < ZimagesNumber);

  return Zimages[imgN];
}

zimg zCreateImage(int w, int h, zPixel col, int comp)
{
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

  for (int64_t i = 0; i <= w*h*comp; i += comp)
  {
    imgs[ZimagesNumber].data[i+0] = col.r;
    if (comp > 1) imgs[ZimagesNumber].data[i+1] = col.g;
    if (comp > 2) imgs[ZimagesNumber].data[i+2] = col.b;
    if (comp > 3) imgs[ZimagesNumber].data[i+3] = col.a;
  }


  delete[] Zimages;
  Zimages = imgs;

  ZimagesNumber++;

  return ZimagesNumber-1;

}

zimg zCreateImage(int w, int h, int comp)
{
  zPixel p(0,0,0);
  return zCreateImage(w, h, p, comp);
}

void zSetImagePixel(zimg imgN, int32_t x, int32_t y, zPixel col)
{
  Zimage_t img = zGetImage(imgN);
  img.data[(y*img.w + x) * img.components + 0] = col.r;
  img.data[(y*img.w + x) * img.components + 1] = col.g;
  img.data[(y*img.w + x) * img.components + 2] = col.b;
  if (img.components > 3) {
    img.data[(y*img.w + x) * img.components + 3] = col.a;
  }
}

void zClearImage(zimg img)
{
  auto image = zGetImage(img);
  memset(&image.data[0], 0, image.components*image.w*image.h);
}


void zClose()
{
#ifndef NO_X11
  glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
#else
  fclose(fb);
  fb = NULL;
#endif
}
