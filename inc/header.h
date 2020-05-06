#ifndef ENDGAME_HEADER
#define ENDGAME_HEADER

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FALSE 0
#define TRUE 1
#define MAX_BULLETS 1000

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

typedef struct {
  SDL_Rect draw_rect;    // dimensions of button
  struct {
    Uint8 r, g, b, a;
  } colour;

  bool pressed;
} button_t;

typedef struct {
  float x, y, dy;
  short life;
  int walking;
  
  SDL_Texture *sheetTexture;
} Man;

typedef struct {
  float x, y, dy;
} Bullet;

#endif
