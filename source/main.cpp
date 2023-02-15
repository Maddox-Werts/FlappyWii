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
#include "background_png.h"
#include "ground_png.h"

// Constants
#define SCR_WIDTH 640
#define SCR_HEIGHT 480

#define PIPE_SPEED 2.5f
#define PIPE_SPACE 75
#define PIPE_CHANCE 150

#define BIRD_GRAV 0.4f
#define BIRD_POWER 8
#define BIRD_SIZE 2

#define CHNK_SIZE 5

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

    // What's our length?
    bodyLength = 5;
    newHeight();
  }

  // Functions
  void newHeight(){
    // Random value
    float randomHeightModi = ((float)(rand() % 10)) / 10.0f;
    y = (randomHeightModi * PIPE_CHANCE) + (SCR_HEIGHT / 2) - (PIPE_CHANCE / 2);
  }
  void move(){
    // Dead?
    if(dead) {return;}

    // Moving
    x -= PIPE_SPEED;

    // Tiling
    if(x < -64){
      x = SCR_WIDTH + 32;
      newHeight();
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
  float frame_time;
  float r;

  int frame;
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

    // Setting frame stuff
    frame_time = 0;
    frame = 0;
  }

  // Functions
  void falling(){
    // Falling
    vy += BIRD_GRAV;
    y += vy;

    // Rotation
    if(vy > 0){   // Down
      r = r+0.125f*((vy * 6)-r);
    }
    else{         // Up
      r = -25;
    }
  }
  void hover(){
    y = (sinf(game_time) * 10) + (SCR_HEIGHT / 2);
  }
  void move(){
    // Dead?
    if(dead){
      if(y < SCR_HEIGHT - (16 * BIRD_SIZE)){
        falling();
      }
      else{
        vy = 0;
        y = SCR_HEIGHT - (16 * BIRD_SIZE);
      }
      return;
    }

    // Falling
    falling();

    // Flapping
    if((wpad_input & WPAD_BUTTON_A)
    && birdPresses > 1){
      vy = -BIRD_POWER;
      frame = 3;
    }

    // Fell into ground?
    if(y + (16 * BIRD_SIZE) > SCR_HEIGHT - 32){
      die();
    }

    // Inc Bird Presses
    birdPresses++;
  }
  void draw(){
    // Switch frame?
    switch(screen){
    case 0:     // Waiting for bird to launch
      if(frame_time > 1.5f){
        frame_time = 0;
        if(frame >= 3)  {frame = 0;}
        else            {frame += 1;}
      }
      else{
        frame_time += 0.1f;
      }
      break;
    case 1:     // Launching!
      if(frame_time > 0.5f){
        frame_time = 0;
        if(frame > 0)   {frame--;}
        else            {frame = 0;}
      }
      else{
        frame_time += 0.1f;
      }
      break;
    }

    // Drawing
    GRRLIB_DrawTile(x,y, sprite, r, BIRD_SIZE,BIRD_SIZE, 0xFFFFFFFF, frame);
  }
  void die(){
    if(dead) {return;}

    dead = true;
    vy = 0;
  }
  void collide(Pipe& pipe){
    if(x + 32 > pipe.x
    && x < pipe.x + 32
    && (y + 32 > pipe.y + PIPE_SPACE
    || y < pipe.y - PIPE_SPACE)){
      die();
    }
  }
};
class GroundChunk{
private:
  // Variables
  GRRLIB_texImg* sprite;

public:
  // Variables
  float x;

  // Constructor
  GroundChunk(int index){
    // Setting position
    x = (index * 32 * CHNK_SIZE);

    // Creating sprite
    sprite = GRRLIB_LoadTexture(ground_png);
    GRRLIB_InitTileSet(sprite, 16,16,0);
  }

  // Functions
  void move(){
    x -= PIPE_SPEED;
    if(x + 32 * CHNK_SIZE <= 0){
      x = SCR_WIDTH;
    }
  }
  void draw(){
    for(int i = 0; i < CHNK_SIZE; i++){   // Remember, 20 tiles == screen width
      GRRLIB_DrawTile(x + i * 32,SCR_HEIGHT-32, sprite, 0, 2,2, 0xFFFFFFFF, 27);
    }
  }
};
class BackgroundChunk{
private:
  // Variables
  GRRLIB_texImg* sprite;

public:
  // Variables
  float x;

  // Constructor
  BackgroundChunk(int index){
    // Loading texture
    sprite = GRRLIB_LoadTexture(background_png);

    // Setting position
    x = index * 320;
  }

  // Functions
  void move(){
    // Moving left
    x -= PIPE_SPEED / 2;

    // Tiling
    if(x + 320 <= 0){
      x = SCR_WIDTH;
    }
  }
  void draw(){
    GRRLIB_DrawImg(x,0, sprite, 0, 1.25f,1.875f, 0xFFFFFFFF);
  }
};

// Functions
/// System
int init(){
  // Initialise the Graphics & Video subsystem
  GRRLIB_Init();

  // Initialise the Wiimotes
  WPAD_Init();

  // Random?
  srand(time(NULL));

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

  // Background
  BackgroundChunk backgrounds[3] = {
    BackgroundChunk(0),
    BackgroundChunk(1),
    BackgroundChunk(2)
  };

  // Ground
  GroundChunk chunks[5] = {
    GroundChunk(0),
    GroundChunk(1),
    GroundChunk(2),
    GroundChunk(3),
    GroundChunk(4)
  };

  // Game loop
  while(running) {
    // Update
    update();
    if(restart()){
      bird = Bird();
      pipes[0] = Pipe(0);
      pipes[1] = Pipe(1);
      for(int i = 0; i < 5; i++) {chunks[i].x = i * 32 * CHNK_SIZE;}
    }

    // Drawing background
    //GRRLIB_DrawImg(SCR_WIDTH/2,0, bckgrnd, 0, 1.25f,1.875f, 0xFFFFFFFF);
    for(int i = 0; i < 3; i++){
      if(!dead) {backgrounds[i].move();}
      backgrounds[i].draw();
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

    // Ground
    for(int i = 0; i < 5; i++) {
      if(!dead) {chunks[i].move();}
      chunks[i].draw();
    }

    // Render the frame buffer to the screen
    GRRLIB_Render();
  }

  // Properly exiting GRRLIB
  GRRLIB_Exit();

  // Exit app
  return 0;
}
