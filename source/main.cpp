// Includes
/// System
#include <stdlib.h>
/// Wii
#include <wiiuse/wpad.h>
/// GRRLIB
#include <grrlib.h>

// Images
#include "bird_png.h"

// Constants
#define SCR_WIDTH 640
#define SCR_HEIGHT 480

// Classes
class Bird{
private:
  // Variables
  float x, y;
  float r;

  GRRLIB_texImg* sprite;

public:
  // Constructor
  Bird(){
    // Creating image
    sprite = GRRLIB_LoadTexture(bird_png);

    // Setting X and Y
    x = 0;
    y = 0;

    // Setting rotation
    r = 0;
  }

  // Functions
  void move(){

  }
  void draw(){
    GRRLIB_DrawImg(x, y, sprite, r, 3, 3, 0xFFFFFFFF);
  }
};

// Variables
bool running = true;

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

  // If [HOME] was pressed on the first Wiimote, break out of the loop
  if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) {running = false;}

  // Exit
  return 0;
}

// Entry point
int main(int argc, char **argv) {
  init();

  // Create bird
  Bird bird;

  // Game loop
  while(running) {
    // Update
    update();

    // Game code..
    bird.move();

    // Render code..
    bird.draw();

    // Render the frame buffer to the screen
    GRRLIB_Render();
  }

  // Properly exiting GRRLIB
  GRRLIB_Exit();

  // Exit app
  return 0;
}