// Includes
/// System
#include <stdlib.h>
/// Wii
#include <wiiuse/wpad.h>
/// GRRLIB
#include <grrlib.h>

// Images
#include "bird_png.h"
#include "pipes_png.h"

// Constants
#define SCR_WIDTH 640
#define SCR_HEIGHT 480

// Variables
u32 wpad_input;
bool running = true;

// Classes
class Bird{
private:
  // Variables
  float x, y;
  float vx, vy;
  float r;

  GRRLIB_texImg* sprite;

public:
  // Constructor
  Bird(){
    // Creating image
    sprite = GRRLIB_LoadTexture(bird_png);

    // Setting X and Y
    x = SCR_WIDTH / 4;
    y = 0;

    // Setting velocity
    vx = 0;
    vy = 0;

    // Setting rotation
    r = 0;
  }

  // Functions
  void move(){
    // Falling
    vy += 0.1f;
    y += vy;

    // Rotation
    r = r+0.1f*((vy * 5)-r);

    // Flapping
    if(wpad_input & WPAD_BUTTON_A){
      vy = -5;
    }
  }
  void draw(){
    GRRLIB_DrawImg(x, y, sprite, r, 3, 3, 0xFFFFFFFF);
  }
};
class Pipe{
private:
  // Variables
  float x, y;
  GRRLIB_texImg* sprite;

public:
  // Constructor
  Pipe(int offset){
    // Loading texture
    sprite = GRRLIB_LoadTexture(pipes_png);
    GRRLIB_InitTileSet(sprite, 32, 29, 0);    // 4x8 image scale

    // Setting position
    x = 50;
    y = 50;
  }

  // Functions
  void move(){

  }
  void draw(){
    // Drawing
    //GRRLIB_DrawImg(x,y, sprite, 0, 1,1, 0xFFFFFFFF);
    GRRLIB_DrawTile(x,y, sprite, 0, 1,1, 0xFFFFFFFF, 2*1);
  }
};

// Functions
/// System
int init(){
  // Initialise the Graphics & Video subsystem
  GRRLIB_Init();

  // Initialise the Wiimotes
  WPAD_Init();

  // Exit
  return 0;
}
int update(){
  // Scan wiimote input
  WPAD_ScanPads();
  WPAD_SetVRes(0, SCR_WIDTH, SCR_HEIGHT);

  // Get input
  wpad_input = WPAD_ButtonsDown(0);

  // If [HOME] was pressed on the first Wiimote, break out of the loop
  if (wpad_input & WPAD_BUTTON_HOME) {running = false;}

  // Exit
  return 0;
}

// Entry point
int main(int argc, char **argv) {
  init();

  // Create bird
  Bird bird;

  // Create pipes
  Pipe pipe(0);

  // Game loop
  while(running) {
    // Update
    update();

    // Game code..
    bird.move();
    pipe.move();

    // Render code..
    bird.draw();
    pipe.draw();

    // Render the frame buffer to the screen
    GRRLIB_Render();
  }

  // Properly exiting GRRLIB
  GRRLIB_Exit();

  // Exit app
  return 0;
}
