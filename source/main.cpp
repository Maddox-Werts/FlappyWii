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

#define PIPE_SPEED 5
#define PIPE_SPACE 70

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
  float bodyLength;
  GRRLIB_texImg* sprite;

public:
  // Constructor
  Pipe(int offset){
    // Loading texture
    sprite = GRRLIB_LoadTexture(pipes_png);
    GRRLIB_InitTileSet(sprite, 32, 29, 0);    // 4x8 image scale

    // Setting position
    x = SCR_WIDTH + 32 + offset;
    y = SCR_HEIGHT / 2;

    // What's our length?
    bodyLength = 5;
  }

  // Functions
  void move(){
    // Moving
    x -= PIPE_SPEED;

    // Tiling
    if(x < -64){
      x = SCR_WIDTH + 32;
    }
  }
  void draw(){
    // Drawing
  // Higher End
    /// Head
    GRRLIB_DrawTile(x,y - PIPE_SPACE, sprite, 0, 2,-2, 0xFFFFFFFF, 2);

    /// Body
    for(int i = 0; i < bodyLength; i++){
      GRRLIB_DrawTile(x,y-(58*(i+1)) - PIPE_SPACE, sprite, 0, 2,-2, 0xFFFFFFFF, 6);
    }

  // Lower end
    /// Head
    GRRLIB_DrawTile(x,y + PIPE_SPACE, sprite, 0, 2,2, 0xFFFFFFFF, 2);

    /// Body
    for(int i = 0; i < bodyLength; i++){
      GRRLIB_DrawTile(x,y+(58*(i+1)) + PIPE_SPACE, sprite, 0, 2,2, 0xFFFFFFFF, 6);
    }
    
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
  Pipe pipes[2] = {Pipe(0), Pipe(1)};

  // Game loop
  while(running) {
    // Update
    update();

    // Game code..
    bird.move();
    for(int i = 0; i < sizeof(pipes); i++){
      pipes[i].move();
    }

    // Render code..
    bird.draw();
    for(int i = 0; i < sizeof(pipes); i++){
      pipes[i].draw();
    }

    // Render the frame buffer to the screen
    GRRLIB_Render();
  }

  // Properly exiting GRRLIB
  GRRLIB_Exit();

  // Exit app
  return 0;
}
