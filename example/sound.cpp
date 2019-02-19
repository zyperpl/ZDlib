#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <thread>
#include <algorithm>

#include "zdlib.h"

#define WIDTH   1920/2
#define HEIGHT  1080/2
#define POINTS_NUM 16

#define COLOR_R 20
#define COLOR_G 120
#define COLOR_B 220

struct Point;

struct World
{
  zVector2<float> mouse;
  Point *points;
  size_t pointsNum;
};

struct Point
{
  zVector2<float> position{static_cast<float>(rand()%WIDTH),static_cast<float>(rand()%HEIGHT)};
  zVector2<float> velocity{static_cast<float>(-100+rand()%200)/100.f, static_cast<float>(-100+rand()%200)/100.f};
  zPixel color{uint8_t(100+rand()%COLOR_R), uint8_t(220+rand()%COLOR_G), uint8_t(30+rand()%COLOR_B)};

  void update(float dt, const World &world)
  {
    auto &x = position.x;
    auto &y = position.y;
    auto &vx = velocity.x;
    auto &vy = velocity.y;

    const auto &mx = world.mouse.x;
    const auto &my = world.mouse.y;

    float airFriction = 1.00001;
    if (zKey(GLFW_KEY_LEFT_SHIFT)) airFriction *= 1.01;

    velocity /= airFriction;
    vy += .1;

    float wallFriction = 0.8;

    if (x < 0       && vx < 0) vx *= -1*wallFriction;
    if (x >= WIDTH  && vx > 0) vx *= -1*wallFriction;
    if (y < 0       && vy < 0) vy *= -1*wallFriction;
    if (y >= HEIGHT && vy > 0) vy *= -1*wallFriction;

    if (velocity.y >  1000.0) vy =  1000.0;
    if (velocity.y < -1000.0) vy = -1000.0;

    float speed = 40;

    if (zKey(ZKEY_UP))   vy -= speed;
    if (zKey(ZKEY_DOWN)) vy += speed;
    
    if (zKey(ZKEY_LEFT))  vx -= speed;
    if (zKey(ZKEY_RIGHT)) vx += speed;
    
    if (zMouseButton(ZMOUSE_BUTTON_LEFT))
    {
      float angle = std::atan2(my-y, mx-x);
      vx += cos(angle)*speed;
      vy += sin(angle)*speed;
    }
    
    position = position+(velocity*dt);
  }

  inline void drawPosition(zVector2<float> pos, zPixel col)
  {
    zDrawPixel((int)(pos.x+0.5), (int)(pos.y+0.5), col);
  }

  inline void draw()
  {
    drawPosition(position, color);
  }
};



int main()
{
  printf("Starting...");
  zCreateWindow(WIDTH, HEIGHT, "ZDLib Sound Test");

  Point points[POINTS_NUM];
  World world;
  world.points = points;
  world.pointsNum = POINTS_NUM;

  zInitAudio();

  float dt = 0.0; 
    
  static auto lastTime = std::chrono::system_clock::now();

  while (zIsOpen())
  {
    auto time = std::chrono::system_clock::now();
    std::chrono::duration<double> timeDiff = time-lastTime; lastTime = time; dt = timeDiff.count();

    zUpdate();

    auto mousePosition = zMousePosition();
    world.mouse.x = mousePosition.x;
    world.mouse.y = mousePosition.y;

    /*
    zVoice(0).enabled   = true;
    zVoice(0).frequency = std::round((float)(world.mouse.x));
    zVoice(0).volume    = ((float)(world.mouse.y)/(float)(HEIGHT)) * 0.5;
    // */

    zClear();

    printf("%f\n", zVoice(0).frequency);

    static int c = 0;
    for (auto &p : points)
    {
      p.update(dt, world);
      p.draw();

      if (c < ZVOICES_N)
      {
        float val = std::pow(p.velocity.x, 2) + std::pow(p.velocity.y, 2);
        val = std::clamp(val, 0.0f, 4000000.0f);
        zVoice(c).enabled   = true;
        zVoice(c).frequency = val;
        zVoice(c).volume    = 0.01;
        c++;
      }
    }
    c = 0;

    if (zKey(GLFW_KEY_SPACE))
    {
      zPixel audioDataColor(255,0,0);
      for (size_t i = 0; i < ZFRAMES-1; ++i)
      {
        int x0 = (float(i)/float(ZFRAMES)) * WIDTH;
        int x1 = (float(i+1)/float(ZFRAMES)) * WIDTH;

        int y0 = HEIGHT/2+zAudioOutput(i)*HEIGHT/2;
        int y1 = HEIGHT/2+zAudioOutput(i+1)*HEIGHT/2;

        zDrawLine(x0,y0, x1,y1, audioDataColor);
      }
    }
    zRender();
  }

  zFree();

}
