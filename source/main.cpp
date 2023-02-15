// Includes
/// System
#include <stdlib.h>
#include <math.h>
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

#define BIRD_GRAV 0.2f
#define BIRD_POWER 8

// Variables
u32 wpad_input;
float game_time = 0;
int screen = 0;
int birdPresses = false;
bool running = true;
bool dead = false;

// Classes
class Pipe{
private:
  // Variables
  float bodyLength;
  GRRLIB_texImg* sprite;

public:
  // Variables
  float x, y;

  // Constructor
  Pipe(int offset){
    // Loading texture
    sprite = GRRLIB_LoadTexture(pipes_png);
    GRRLIB_InitTileSet(sprite, 32, 29, 0);    // 4x8 image scale

    // Setting position
    x = SCR_WIDTH + 32 + (offset * 64 * 6);
    y = 1 * (SCR_HEIGHT / 2);

    // What's our length?
    bodyLength = 5;
  }

  // Functions
  void move(){
    // Dead?
    if(dead) {return;}

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
class Bird{
private:
  // Variables
  float x, y;
  float vx, vy;
  float ft;
  float r;

  GRRLIB_texImg* sprite;

public:
  // Constructor
  Bird(){

    // Creating image
    sprite = GRRLIB_LoadTexture(bird_png);
    GRRLIB_InitTileSet(sprite, 16,16, 0);

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
  void falling(){
    // Falling
    vy += BIRD_GRAV;
    y += vy;

    // Rotation
    r = r+0.1f*((vy * 5)-r);
  }
  void hover(){
    y = (sinf(game_time) * 10) + (SCR_HEIGHT / 2);
  }
  void move(){
    // Dead?
    if(dead){
      if(y < SCR_HEIGHT - 48){
        falling();
      }
      else{
        vy = 0;
      }
      return;
    }
    
    // Falling
    falling();

    // Flapping
    if((wpad_input & WPAD_BUTTON_A)
    && birdPresses > 1){
      vy = -BIRD_POWER;
    }

    // Inc Bird Presses
    birdPresses++;
  }
  void draw(){
    //GRRLIB_DrawImg(x, y, sprite, r, 3, 3, 0xFFFFFFFF);
    GRRLIB_DrawTile(x,y, sprite, r, 3,3, 0xFFFFFFFF, 0);
  }
  void die(){
    if(dead) {return;}

    dead = true;
    vy = 0;
  }
  void collide(Pipe& pipe){
    if(x + 48 > pipe.x
    && x < pipe.x + 32
    && (y + 48 > pipe.y + PIPE_SPACE
    || y < pipe.y - PIPE_SPACE)){
      die();
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
/// Game
int restart(){
  if(!dead) {return 0;}

  if(wpad_input & WPAD_BUTTON_A){
    birdPresses = 0;
    dead = false;
    return 1;
  }
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
    if(restart()){
      bird = Bird();
      pipes[0] = Pipe(0);
      pipes[1] = Pipe(1);
    }

    // Screens?
    switch(screen){
    case 0:   // Bird waiting to launch
      // Game code..
      game_time += 0.1f;
      bird.hover();

      /// Switching scenes
      if(wpad_input & WPAD_BUTTON_A){
        screen = 1;
      }

      // Render code..
      bird.draw();
      break;
    case 1:   // Launching!
      // Game code..
      bird.move();
      for(int i = 0; i < 2; i++){
        bird.collide(pipes[i]);
        pipes[i].move();
      }

      // Render code..
      bird.draw();
      for(int i = 0; i < 2; i++){
        pipes[i].draw();
      }
      break;
    }

    // Render the frame buffer to the screen
    GRRLIB_Render();
  }

  // Properly exiting GRRLIB
  GRRLIB_Exit();

  // Exit app
  return 0;
}
