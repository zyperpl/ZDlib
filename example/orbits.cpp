#include <vector>
#include <cmath>
#include <thread> 
#include <mutex>

#include "zdlib.h"

// window buffer size
#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

// window scale size
#define WINDOW_SCALE  1

#define PARTICLES_N 1000

#define INITIAL_V 4

zimg screen = zCreateImage(SCREEN_WIDTH, SCREEN_HEIGHT);

std::mutex drawMutex;

bool keyPressed = false;

bool isRunning = true;
const zVector2<double> CENTER = { SCREEN_WIDTH/2.0F, SCREEN_HEIGHT/2.0F };

double angle(zVector2<double> pos1, zVector2<double> pos2)
{
  return atan2(pos2.y - pos1.y, pos2.x - pos1.x);
}

double distancePower(zVector2<double> pos1, zVector2<double> pos2)
{
  return (pow(pos1.x-pos2.x, 2) + pow(pos1.y-pos2.y, 2));
}



class Particle
{
  private:
    zVector2<double> position;
    zVector2<double> velocity;
    zPixel color;
  public:
    Particle(zPixel c) 
    { 
      position = { 
        (double)(rand()%SCREEN_WIDTH), 
        (double)(rand()%SCREEN_HEIGHT)
      }; 

      velocity = { 
        -INITIAL_V+(double)(rand()%(INITIAL_V*2)), 
        -INITIAL_V+(double)(rand()%(INITIAL_V*2))
      }; 
      velocity.x /= 60.0F;
      velocity.y /= 60.0F;

      color = c;
    }

    void setPosition(zVector2<double> pos)
    {
      this->position = pos;
    }

    void setVelocity(zVector2<double> vel)
    {
      this->velocity = vel;
    }

    void draw() 
    { 
      if (position.x > 0 && position.x < SCREEN_WIDTH
      &&  position.y > 0 && position.y < SCREEN_HEIGHT)
      {
        // draw pixel to buffer
        zSetImagePixel(screen,
            (int)(position.x+0.5), 
            (int)(position.y+0.5), 
            color
        );
      }
    }

    void update() 
    { 
      float dist = distancePower(position, CENTER);

      if (dist < 2) dist = 2;

      velocity.x += cos(angle(position, CENTER ) )/(dist);
      velocity.y += sin(angle(position, CENTER ) )/(dist);

      if (abs(velocity.x) > 5) velocity.x /= 2;
      if (abs(velocity.y) > 5) velocity.y /= 2;

      position.x += velocity.x; 
      position.y += velocity.y; 

      //if (position.x < 0 || position.x > SCREEN_WIDTH)  velocity.x *= -0.1F;
      //if (position.y < 0 || position.y > SCREEN_HEIGHT) velocity.y *= -0.1F;
    }


};

void movePosition(zVector2<int> *position)
{
  position->x += -1+rand()%3;
  position->y += rand()%2;
}

void updateParticles(std::vector<Particle*> *parts)
{
  while (isRunning)
  {
    size_t partsN = parts->size();
    if (drawMutex.try_lock())
    {
      for (int i = partsN-1; i >= 0; i--)
      {
        parts->at(i)->update();

      }
      drawMutex.unlock();
    }
    
    std::this_thread::sleep_for
        ( std::chrono::milliseconds(1));
  }
}

void drawParticles(std::vector<Particle*> *parts)
{
  while (isRunning)
  {
    size_t partsN = parts->size();
    drawMutex.lock();
    for (int i = partsN-1; i >= 0; i--)
    {
      parts->at(i)->draw();

    }
    drawMutex.unlock();

    zDrawImage(screen, 0, 0);
    
    std::this_thread::sleep_for
        ( std::chrono::milliseconds(0));

    // refresh screen only when key is not pressed
    //if (!keyPressed) zClear();

  }
}

void breakImage(const char *name, std::vector<Particle*> *parts)
{
  Zimage_t img = zGetImage(zLoadImage(name));

  //int x_offset = SCREEN_WIDTH/2  - img.w/2;
  //int y_offset = SCREEN_HEIGHT/2 - img.h/2;
  int x_offset = 0;
  int y_offset = 0;

  for (int y = 0; y < img.h; ++y)
    for (int x = 0; x < img.w; ++x)
  {
    int i = (x+y*img.w)*img.components;
    Particle *p = new Particle({ img.data[i+0], img.data[i+1], img.data[i+2] });
    p->setPosition( { (double)x_offset+x, (double)y_offset+y } );
    p->setVelocity( { 0.F, 0.F } );
    parts->push_back(p);
  }

  printf("image: %s (%d;%d)\n", img.name, img.w, img.h);
}

int main(void)
{
  std::vector<Particle*> particles;
  for (size_t i = 0; i < PARTICLES_N; i++) 
  {
    particles.push_back(new Particle({ (uint8_t)rand(), (uint8_t)rand(), (uint8_t)rand() }) );
  }

  //breakImage("lena.png", &particles);

  // create window
  zCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ZDLib Demo", WINDOW_SCALE);

  std::thread updateThread(updateParticles, &particles);
  std::thread drawThread  (drawParticles,   &particles);

  // window is open and demo is running
  while (zIsOpen() && isRunning) 
  {
    // stop demo when ESCAPE key is pressed
    if (zKey(ZKEY_QUIT)) isRunning = false;

    // place pixels under mouse
    if (zMouseButton(ZMOUSE_BUTTON_LEFT))
    {
      auto mousePosition = zGetMousePosition();
      mousePosition.x /= (double)WINDOW_SCALE;
      mousePosition.y /= (double)WINDOW_SCALE;

      auto part = new Particle({ (uint8_t)(rand()%255), (uint8_t)(rand()%128), (uint8_t)(rand()%20) });
      part->setPosition(mousePosition);
      particles.push_back(part);

      printf("Mouse position: %2.2f; %2.2f\n", mousePosition.x, mousePosition.y);
    }

    // save state of spacebar key to global variable
    keyPressed = zKey(ZKEY_ACTION);

    // swap buffers and polls window events
    zUpdate();
    zRender();
  }
  isRunning = false;

  // wait for update thread
  updateThread.join();

  // wait for draw thread
  drawThread.join();

  // free all lib resources 
  zFree();

  return 0;
}

