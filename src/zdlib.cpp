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

#ifdef ZAUDIO
  static ZaudioData audioData;
  static PaStream *audioStream = nullptr;
  static float audioOutputBuffer[ZFRAMES];
#endif

/*
 * window
 */


// shaders
const GLchar* z_basicVertexSource = R"glsl(
  #ifdef GL_ES
    precision highp float;
  #endif

  attribute vec2 position;

  varying vec2 uv;

  void main()
  {
    gl_Position = vec4(position, 1.0, 1.0);
    uv = gl_Position.xy / 2.0 + 0.5;
    uv.y = 1.0-uv.y;
  }
)glsl";
const GLchar* z_basicFragmentSource = R"glsl(
  #ifdef GL_ES
    precision highp float;
  #endif
  varying vec2 uv;
  uniform sampler2D texture;
  void main()
  {
    gl_FragColor = texture2D(texture, uv);
  }
)glsl";

zPixel zRGB (uint8_t r, uint8_t g, uint8_t b)            { return zPixel(r, g, b, 255); }
zPixel zRGB (uint8_t c)                                  { return zPixel(c, c, c, 255); }
zPixel zRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { return zPixel(r, g, b, a);   }
zPixel zRGB (float r, float g, float b)                  { return zPixel(r*255,g*255,b*255, 255); }
zPixel zRGB (double r, double g, double b)               { return zPixel(g*255,g*255,b*255, 255); }

Zwindow_t *zCreateWindow(uint32_t w, uint32_t h, const char *name, uint8_t scale)
{
  // initialize Zwindow
  Zwindow = new Zwindow_t();
  Zwindow->width = w;
  Zwindow->height = h;
  Zwindow->components = Z_COMPONENTS;

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


  #ifdef OPENGL_ERROR_CALLBACK

  auto OpenGLMessageCallback = [](GLenum source, GLenum type, GLuint id, GLenum severity, 
                                  GLsizei length, const GLchar* message, const void* userParam)
  {
    if (type == GL_DEBUG_TYPE_ERROR && severity != 0x9146)
    {
      static long errors = 0;
      fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
              ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
                type, severity, message);

      errors++;
      if (errors >= 64) exit(3);
    }
  };

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(OpenGLMessageCallback, 0);

  #endif

  // create vertices
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);

  GLfloat vertices[] = { 
    1.0, -1.0, 
    -1.0, -1.0, 
    -1.0, 1.0, 
    1.0, 1.0 
  };

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint ebo;
  glGenBuffers(1, &ebo);

  GLuint elements[] = {
      0, 1, 2,
      0, 2, 3
  };

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  GLuint shaderProgram = zShader();
  
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

  //GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
  //glEnableVertexAttribArray(texAttrib);
  //glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

  // create texture
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  memset(&Zwindow->buffer[0], 0, w * h * Zwindow->components);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, 
               GL_RGB, GL_UNSIGNED_BYTE, &Zwindow->buffer[0]);
    
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

#else
  fb = fopen(FB_FILE, "wb");
  if (!fb) fprintf(stderr, "Cannot read framebuffer %s!\n", FB_FILE);

  //input = fopen(KB_FILE, "rb");
  input = open(KB_FILE, O_NONBLOCK | O_RDONLY);
  if (!input) fprintf(stderr, "Cannot read keyboard input %s!\n", KB_FILE);
#endif
  return Zwindow;
}

int zShader(const char *vertexFileName, const char *fragmentFileName)
{
#ifndef NO_X11
  auto printErrors = [](void (*fInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*), GLuint sop) -> int
  {
    int logLength; 
    char *log = new char[ZREAD_FILE_BUFFER]; 
    fInfoLog(sop, ZREAD_FILE_BUFFER, &logLength, log); 
   
    if (logLength > 0) 
    {
      printf("Compilation error:\n%s\n", log); 
    }
    
    delete[] log;
    return logLength;
  };

  std::string vertexSourceStr   = z_basicVertexSource;
  std::string fragmentSourceStr = z_basicFragmentSource;

  if (vertexFileName != nullptr)
  {
    vertexSourceStr = zReadFile(vertexFileName).c_str();
  }
  if (fragmentFileName != nullptr)
  {
    fragmentSourceStr = zReadFile(fragmentFileName);
  }
  
  GLuint vertexShader   = 0;
  GLuint fragmentShader = 0;
  
  const char *vertexSource = vertexSourceStr.c_str();
  const char *fragmentSource = fragmentSourceStr.c_str();

  // create shaders
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);
  if (printErrors(glGetShaderInfoLog, vertexShader) > 0) return 0;

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);
  if (printErrors(glGetShaderInfoLog, fragmentShader) > 0) return 0;

  static GLuint shaderProgram = 0;
  if (shaderProgram > 0)
  {
    glDeleteProgram(shaderProgram);
  }

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  printErrors(glGetProgramInfoLog, shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glUseProgram(shaderProgram);

  return shaderProgram;
#endif
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

void zClear(int mode)
{
  int w = Zwindow->width;
  int h = Zwindow->height;

#ifdef NO_X11
  w = FB_WIDTH;
  h = FB_HEIGHT;
#endif

  if (mode >= 1)
  {
    if (Zwindow) memset(&Zwindow->buffer[0], 0, w*h*Z_COMPONENTS);
  }
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

  auto pixelFormat = GL_RGB;
  if (Z_COMPONENTS > 3) pixelFormat = GL_RGBA;

  //glClearColor(1.0, 1.0, 0.01, 0.5);

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Zwindow->width, Zwindow->height, 
               pixelFormat, GL_UNSIGNED_BYTE, &Zwindow->buffer[0]);

  // TODO: add option to invoke setting uniform values from external code
  static float time = 0.0;
  time += 0.03;
  glUniform1f(1, time);

  int width, height;
  glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

  double r  = double(width)  / double(Zwindow->width);
  double r2 = double(height) / double(Zwindow->height);

  if (r > r2) r = r2;
  int w = int(r*Zwindow->width);
  int h = int(r*Zwindow->height);
  glViewport((width-w)/2, (height-h)/2, w, h);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

#else
  const int SIZE = FB_WIDTH * FB_HEIGHT * Z_COMPONENTS;

  // RGB to BGR
  #pragma omp parallel for
  for (int i = 0; i < SIZE; i+=Z_COMPONENTS)
  {
    auto t = Zwindow->buffer[i+0];
    Zwindow->buffer[i+0] = Zwindow->buffer[i+2];
    Zwindow->buffer[i+2] = t;
  }
  
  int written = fwrite(&Zwindow->buffer[0], sizeof(uint8_t), SIZE, fb);
  fclose(fb); fopen(FB_FILE, "wb"); // XXX
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

#ifdef ZAUDIO
  Pa_CloseStream(audioStream);
  Pa_Terminate();
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

zVector2<double> zMousePosition()
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




static int zAudioCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long frames,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData)
{
  (void)(userData);

  ZaudioData *data = static_cast<ZaudioData*>(userData);
  float *out = static_cast<float*>(outputBuffer);

  while (frames --> 0)
  {
    unsigned long f = ZFRAMES-frames-1;

    *out = 0.0;

    for (Zvoice &v : data->voice)
    {
      if (v.enabled)
      {
        float val = 2*M_PI*v.frequency*((float)(v.phase)/(float)(ZSAMPLE_RATE));
        *out += std::sin(val)*v.volume;
        v.phase ++;
      }
    }

    audioOutputBuffer[f] = *out;

    out++;
  }
    
  return 0;
}

int zInitAudio(int channels, long sampleRate, long frames)
{
  // initialize audio
  if (Pa_Initialize() != paNoError)
  {
    fprintf(stderr, "Error while initializing audio!\n");
    return 1;
  }


  if (Pa_OpenDefaultStream(&audioStream, 0, channels, paFloat32, sampleRate, frames, zAudioCallback, &audioData))
  {
    fprintf(stderr, "Cannot open audio stream!\n");
    return 2;
  }
  Pa_StartStream(audioStream);

  return 0;
}

Zvoice &zVoice(size_t id)
{
  return audioData.voice[id];
}

float zAudioOutput(size_t idx)
{
  return audioOutputBuffer[idx];
}


void zDrawPixel(uint8_t *buffer, uint16_t w, uint16_t h, int components, uint16_t x, uint16_t y, const zPixel &color)
{
  long long p = (x+y*w)*components;
  buffer[p + 0] = color.r;
  if (components > 1) buffer[p + 1] = color.g;
  if (components > 2) buffer[p + 2] = color.b;  
  if (components > 3) buffer[p + 3] = color.a;
}

void zDrawPixel(zimg img, uint16_t x, uint16_t y, const zPixel &color)
{
  auto image = zGetImage(img);
  if (x >= image.w) return;
  if (y >= image.h) return;

  zDrawPixel(image.data, image.w, image.h, image.components, x, y, color);
}

void zDrawPixel(Zwindow_t *wnd, uint16_t x, uint16_t y, const zPixel &color)
{
  if (x >= wnd->width)  return;
  if (y >= wnd->height) return;

  int w = wnd->width;
  int h = wnd->height;
  
  #ifdef NO_X11
    w = FB_WIDTH;
    h = FB_HEIGHT;
  #endif

  zDrawPixel(wnd->buffer, w, h, wnd->components, x, y, color);
}

void zDrawPixel(uint16_t x, uint16_t y, const zPixel &color)
{
  zDrawPixel(Zwindow, x, y, color);
}


void zDrawImage(uint8_t *buffer, const uint16_t w, const uint16_t h, const int components, 
                const uint16_t x, const uint16_t y, const zimg img, double xScale, double yScale)
{
  const Zimage_t *image = &Zimages[img];
  
  if (x+image->w < 0) return;
  if (x >= w)         return;
  if (y >= h)         return;
  if (y+image->h < 0) return;
  
  const int niw = image->w * std::abs(xScale);
  const int nih = image->h * std::abs(yScale);

  #pragma omp parallel for
  for (int i = 0; i < niw*nih; i++)
  {
    int bx = i%niw;
    int by = i/niw;

    if (x+bx >= w) continue;
    if (y+by >= h) continue;

    if (x+bx < 0) continue;
    if (y+by < 0) continue;

    int ix = bx/std::abs(xScale);
    int iy = by/std::abs(yScale);

    if (xScale < 0) ix = image->w-ix;
    if (yScale < 0) iy = image->h-iy;

    int bufidx = (bx+by*w+(x+y*w))*components;
    int imgidx = (ix+iy*image->w)*image->components;

    for (int c = 0; c < components; c++)
    {
      uint8_t v = 0;
      
      if (image->components > c) v = image->data[imgidx+c];

      buffer[bufidx + c] = v;
    }
  }

}

void zDrawImage(zimg target, uint16_t x, uint16_t y, const zimg source, double xScale, double yScale)
{
  auto image = zGetImage(target);
  
  zDrawImage(image.data, image.w, image.h, image.components, x, y, source, xScale, yScale);
}

void zDrawImage(Zwindow_t *wnd, uint16_t x, uint16_t y, const zimg img, double xScale, double yScale)
{
  uint16_t w = wnd->width;
  uint16_t h = wnd->height;

  #ifdef NO_X11
    w = FB_WIDTH;
    h = FB_HEIGHT;
  #endif

  zDrawImage(wnd->buffer, w, h, wnd->components, x, y, img, xScale, yScale);
}

void zDrawImage(uint16_t x, uint16_t y, const zimg img, double xScale, double yScale)
{
  zDrawImage(Zwindow, x, y, img, xScale, yScale);
}

void zDrawImage(zimg target, uint16_t x, uint16_t y, const char *imgName, double xScale, double yScale)
{
  auto source = zLoadImage(imgName);
  auto image = zGetImage(target);
  zDrawImage(image.data, image.w, image.h, image.components, x, y, source, xScale, yScale);
}

void zDrawImage(Zwindow_t *wnd, uint16_t x, uint16_t y, const char *imgName, double xScale, double yScale)
{
  auto img = zLoadImage(imgName);
  zDrawImage(wnd, x, y, img, xScale, yScale);
}

void zDrawImage(uint16_t x, uint16_t y, const char *imgName, double xScale, double yScale)
{
  zDrawImage(Zwindow, x, y, zLoadImage(imgName), xScale, yScale);
}

void zDrawLine(uint8_t *buffer, uint16_t w, uint16_t h, int components, 
               uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const zPixel &color)
{
  auto putPx = [w, h, components, buffer](int32_t x, int32_t y, zPixel c)
  {
    if (x < 0)  return;
    if (y < 0)  return;
    if (x >= w) return;
    if (y >= h) return;

    long long p = (x + y*w) * components;
    buffer[p + 0] = c.r;
    buffer[p + 1] = c.g;
    buffer[p + 2] = c.b;  
  };

	bool yLonger = false;
	int incrementVal;

	int shortLen = y2-y1;
	int longLen  = x2-x1;

	if (abs(shortLen)>abs(longLen)) 
  {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;
		yLonger=true;
	}

	if (longLen<0) incrementVal = -1;
	else incrementVal = 1;

	double divDiff;
	if (shortLen == 0) divDiff = longLen;
	else divDiff = (double)longLen / (double)shortLen;
	if (yLonger) 
  {
		for (int i = 0; i != longLen; i += incrementVal) 
    {
			putPx(x1 + (int)((double)i/divDiff), y1+i, color);
		}
	} else 
  {
		for (int i = 0; i != longLen; i += incrementVal) 
    {
			putPx(x1+i, y1 + (int)((double)i/divDiff), color);
		}
	}
}


void zDrawLine(zimg img, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const zPixel &color)
{
  auto image = zGetImage(img);
  zDrawLine(image.data, image.w, image.h, image.components, x1, y1, x2, y2, color);
}

void zDrawLine(Zwindow_t *wnd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const zPixel &color)
{
  int w = wnd->width;
  int h = wnd->height;

#ifdef NO_X11
  w = FB_WIDTH;
  h = FB_HEIGHT;
#endif
  zDrawLine(wnd->buffer, w, h, wnd->components, x1, y1, x2, y2, color);
}

void zDrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const zPixel &color)
{
  zDrawLine(Zwindow, x1, y1, x2, y2, color);
}


zPixel zGetPixel(const uint8_t *buffer, const uint8_t w, const uint8_t h, const uint8_t components, uint16_t x, uint16_t y)
{
  if (x < 0 || y < 0) return zRGB(0);
  if (x >= w) return zRGB(0);
  if (y >= h) return zRGB(0);
  
  zPixel ret;
  const int idx = (x+y*w)*components;
  ret.r = buffer[idx+0];
  ret.g = components > 1 ? buffer[idx+1] : buffer[idx];
  ret.b = components > 2 ? buffer[idx+2] : buffer[idx];
  ret.a = components > 3 ? buffer[idx+3] : buffer[idx];
  return ret;
}

zPixel zGetPixel(const Zwindow_t *wnd, const uint16_t x, const uint16_t y)
{
  int w = wnd->width;
  int h = wnd->height;

#ifdef NO_X11
  w = FB_WIDTH;
  h = FB_HEIGHT;
#endif
  
  return zGetPixel(wnd->buffer, w, h, wnd->components, x, y);
}

zPixel zGetPixel(const uint16_t x, const uint16_t y)
{
  return zGetPixel(Zwindow, x, y);
}

zPixel zGetPixel(const zimg img, const uint16_t x, const uint16_t y)
{
  auto image = zGetImage(img);
  return zGetPixel(image.data, image.w, image.h, image.components, x, y);
}


zPixel zGetPixel(const uint8_t *buffer, const uint8_t w, const uint8_t h, const uint8_t components, double x, double y)
{
  // linear interpolation
  auto leftUp    = zGetPixel(buffer, w, h, components, (uint16_t)x,      (uint16_t)y);
  auto rightUp   = zGetPixel(buffer, w, h, components, (uint16_t)(x+.5), (uint16_t)y);
  auto leftDown  = zGetPixel(buffer, w, h, components, (uint16_t)x,      (uint16_t)(y+0.5));
  auto rightDown = zGetPixel(buffer, w, h, components, (uint16_t)(x+.5), (uint16_t)(y+0.5));

  double ratioX = x-(int)(x);
  double ratioY = y-(int)(y);
  
  auto up   = leftUp   * (1.0-ratioX) + rightUp   * ratioX;
  auto down = leftDown * (1.0-ratioX) + rightDown * ratioX;

  auto ret = up * (1.0-ratioY) + down * ratioY;

  return ret;
}

zPixel zGetPixel(const Zwindow_t *wnd, const double x, const double y)
{
  int w = wnd->width;
  int h = wnd->height;

#ifdef NO_X11
  w = FB_WIDTH;
  h = FB_HEIGHT;
#endif
  
  return zGetPixel(wnd->buffer, w, h, wnd->components, x, y);
}

zPixel zGetPixel(const double x, const double y)
{
  return zGetPixel(Zwindow, x, y);
}

zPixel zGetPixel(const zimg img, const double x, const double y)
{
  auto image = zGetImage(img);
  return zGetPixel(image.data, image.w, image.h, image.components, x, y);
}

std::string zReadFile(const char *fileName)
{
  FILE *f = fopen(fileName, "rb");
  if (!f) fprintf(stderr, "Cannot read file %s!\n", fileName);

  fseek(f, 0, SEEK_END);
  size_t fileSize = ftell(f);
  rewind(f);

  char *buf = new char[fileSize+1];
  memset(&buf[0], 0, fileSize+1);
  fread(&buf[0], fileSize, 1, f);

  std::string str(&buf[0], fileSize);
  delete[] buf;

  return str;
}
