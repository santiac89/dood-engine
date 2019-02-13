// SDL2 Hello, World!
// This should display a white screen for 2 seconds
// compile with: clang++ main.cpp -o hello_sdl2 -lSDL2
// run with: ./hello_sdl2
#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "math_utils.h"
#include "string_utils.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_CENTER_H (SCREEN_WIDTH / 2)
#define SCREEN_CENTER_V (SCREEN_HEIGHT / 2)
#define hfov M_PI / 4.0f  // Affects the horizontal field of vision
#define vfov (.2f*SCREEN_HEIGHT)    // Affects the vertical field of vision
#define MAX_HEIGHT 1024
#define FINEANGLES		8192

using namespace std;

class Polygon {
  public:
    int x1;
    int x2;
    int y1;
    int y2;

    Polygon(int _x1, int _y1, int _x2, int _y2) {
      x1 = _x1;
      x2 = _x2;
      y1 = _y1;
      y2 = _y2;
    }
};

class Player {
  public:
    int x;
    int y;
    float angle;    
};

vector<int> viewangletox(hfov * 65535);

void initTables() {
  int fovSize = viewangletox.size();
  int steps = fovSize / SCREEN_WIDTH;

  for (int i = 0; i < SCREEN_WIDTH; i++) {
    for (int o = 0; o < steps; o++) {
      viewangletox[(steps * i) + o] = i; 
    }
  }
}

vector<Polygon*> loadPolygons() {
  vector<Polygon*> polygons;

  ifstream file ("points.plain"); //file just has some sentences

  if (!file) {
    cout << "unable to open file";
    return polygons;
  }

  string line;

  while (getline (file, line)){
    vector<string> parts = split(line, ",");

    Polygon *polygon = new Polygon(stoi(parts[0]),stoi(parts[1]),stoi(parts[2]),stoi(parts[3]));
    polygons.push_back(polygon);
  }

  return polygons;
}

void DrawPlayer2D(SDL_Renderer* renderer, Player player) {
  float leftFOV = player.angle - hfov;
  float rightFOV = player.angle + hfov;
  vector<float> playerVector = angleToVector(player.angle);
  vector<float> leftFOVVector = angleToVector(leftFOV);
  vector<float> rightFOVVector = angleToVector(rightFOV);

  // Draw Player 
  SDL_RenderDrawLine(renderer, SCREEN_CENTER_H, SCREEN_CENTER_V, SCREEN_CENTER_H, SCREEN_CENTER_V - 15);
  SDL_RenderDrawPoint(renderer, player.x, player.y);
  SDL_SetRenderDrawColor(renderer, 20, 255, 255, SDL_ALPHA_OPAQUE);  
  SDL_RenderDrawLine(renderer, player.x, player.y, player.x + playerVector[0] * 30, player.y + playerVector[1] * 30);
  SDL_SetRenderDrawColor(renderer, 255, 255, 20, SDL_ALPHA_OPAQUE);  
  SDL_RenderDrawLine(renderer, player.x, player.y, player.x + playerVector[0] * 20, player.y + playerVector[1] * 20);

  // Draw FOV
  SDL_SetRenderDrawColor(renderer, 255, 0, 255, SDL_ALPHA_OPAQUE);  

  SDL_RenderDrawLine(renderer, player.x, player.y, player.x + leftFOVVector[0] * 50, player.y + leftFOVVector[1]* 50);
  SDL_RenderDrawLine(renderer, player.x, player.y, player.x + rightFOVVector[0] * 50, player.y + rightFOVVector[1]* 50);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);  
}

void DrawPolygon(SDL_Renderer* renderer, Player player, Polygon* polygon) {
  vector<float> leftWallVector(2);
  vector<float> rightWallVector(2);

  int leftDistance = distance(player.x, player.y, polygon->x1, polygon->y1);
  int rightDistance = distance(player.x, player.y, polygon->x2, polygon->y2);
  vector<float> playerVector = angleToVector(player.angle);

  leftWallVector[0] = polygon->x1 - player.x;
  leftWallVector[1] = polygon->y1 - player.y;
  leftWallVector = unitVector(leftWallVector);

  rightWallVector[0] = polygon->x2 - player.x;
  rightWallVector[1] = polygon->y2 - player.y;
  rightWallVector = unitVector(rightWallVector);


  // Draw 2D line polygon
  SDL_RenderDrawLine(renderer, polygon->x1, polygon->y1, polygon->x2, polygon->y2);
  


  float leftFOV = player.angle - hfov;
  float rightFOV = player.angle + hfov;
  vector<float> leftFOVVector = angleToVector(leftFOV);
  vector<float> rightFOVVector = angleToVector(rightFOV);
  double angleToLeft = angle(leftFOVVector, leftWallVector);
  double angleToRight = angle(leftFOVVector, rightWallVector);

  // SDL_RenderDrawLine(renderer, player.x, player.y, player.x + leftWallVector[0] * 50, player.y + leftWallVector[1]* 50);
  // SDL_RenderDrawLine(renderer, player.x, player.y, player.x + rightWallVector[0] * 50, player.y + rightWallVector[1]* 50);


  double leftWallX; 
  double rightWallX;

  // if (angleToLeft < 0  && angleToRight < 0) {
    // return;
  // }

  if (angleToLeft < 0 || angleToLeft > hfov) {
    leftWallX = 0;
  } else {
    leftWallX = viewangletox[angleToLeft * 65535];
  }

  if (angleToRight < 0 || angleToRight > hfov) {
    rightWallX = SCREEN_WIDTH;
  } else {
    rightWallX = viewangletox[angleToRight * 65535];
  }

  if (angleToLeft < 0 && angleToRight > hfov) {
    leftWallX = 0;
    rightWallX = 0;
  }
  // cout << "ANGLE TO LEFT: " << angleToLeft << endl;
  // cout << "ANGLE TO RIGHT: " << angleToRight << endl;


  // cout << "LEFT WALL X: " << leftWallX << endl;
  // cout << "RIGHT WALL X: " << rightWallX << endl;

  double leftWallY1 = SCREEN_CENTER_V - (MAX_HEIGHT / leftDistance * 15);
  double leftWallY2 = SCREEN_CENTER_V + (MAX_HEIGHT / leftDistance * 15);
  double rightWallY1 = SCREEN_CENTER_V - (MAX_HEIGHT / rightDistance * 15);
  double rightWallY2 = SCREEN_CENTER_V + (MAX_HEIGHT / rightDistance * 15);

  // if (rightWallX > SCREEN_WIDTH) {
    // rightWallX = SCREEN_WIDTH;
    // rightWallY1 = 0;
    // rightWallY2 = SCREEN_HEIGHT;
  // }

  // if (leftWallX < 0) {
    // leftWallX = 0;
    // leftWallY1 = 0;
    // leftWallY2 = SCREEN_HEIGHT;
  // }


  // Draw player vector

  // Draw vertical lines
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);  
  SDL_RenderDrawLine(renderer, leftWallX, leftWallY1, leftWallX, leftWallY2);
  SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);  
  SDL_RenderDrawLine(renderer, rightWallX, rightWallY1, rightWallX, rightWallY2);
  
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);  
  // Draw horizontal lines
  SDL_RenderDrawLine(renderer, leftWallX, leftWallY1, rightWallX, rightWallY1);
  SDL_RenderDrawLine(renderer, leftWallX, leftWallY2, rightWallX, rightWallY2);
}




int main(int argc, char* args[]) {
  vector<Polygon*> polygons = loadPolygons();
  SDL_Window* window = NULL;
  SDL_Surface* screenSurface = NULL;
  SDL_Renderer* renderer = NULL; 
  initTables();

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
    return 1;
  }

  window = SDL_CreateWindow(
			    "hello_sdl2",
			    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			    SCREEN_WIDTH, SCREEN_HEIGHT,
			    SDL_WINDOW_SHOWN
			    );
          
  if (window == NULL) {
    fprintf(stderr, "could not create window: %s\n", SDL_GetError());
    return 1;
  }

  renderer = SDL_CreateRenderer(window, 0, 0);
  screenSurface = SDL_GetWindowSurface(window);

  Player player;

  player.x = 120;
  player.y = 180;
  player.angle = 0.0f;
  unsigned int lastTime = 0, currentTime, elapsedTime;

  currentTime = SDL_GetTicks();
  bool running = true;

  while (running) {
    currentTime = SDL_GetTicks();
    elapsedTime = currentTime - lastTime;
    lastTime = currentTime;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);  

    SDL_PumpEvents();
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);

    if ( keystate[SDL_SCANCODE_D] ) {
      player.angle += 0.001f * elapsedTime;
    } else if ( keystate[SDL_SCANCODE_A] ) {
      player.angle -= 0.001f * elapsedTime;
    } else if (keystate[SDL_SCANCODE_ESCAPE]) {
      running = false;
    }

    for (int i = 0; i < polygons.size(); i++) {
      Polygon *polygon = polygons[i];
      DrawPolygon(renderer, player, polygon);
    }

    DrawPlayer2D(renderer, player);
    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  return 0;
}


