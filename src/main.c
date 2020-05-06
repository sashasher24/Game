#include <SDL2/SDL.h>
#include "SDL_image.h"
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define MAX_BULLETS 1000

typedef struct {
  float x, y, dy;
  short life;
  char *name;
  int currentSprite, walking, facingLeft, shooting, visible;
  int alive;
  
  SDL_Texture *sheetTexture;
} Man;

typedef struct {
  float x, y, dy;
} Bullet;

SDL_Texture *bulletTexture;
SDL_Texture *backgroundTexture;
Bullet *bullets[MAX_BULLETS] = { NULL };
// Man enemy;

int globalTime = 0;

void addBullet(float x, float y, float dy) {
  int found = -1;
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i] == NULL) {
      found = i;
      break;
    }
  }

  if (found >= 0) {
    int i = found;
    bullets[i] = malloc(sizeof(Bullet));
    bullets[i]->x = x;
    bullets[i]->y = y;
    bullets[i]->dy = dy;
  }
}

void removeBullet(int i) {
  if (bullets[i]) {
    free(bullets[i]);
    bullets[i] = NULL;
  }
}

int processEvents(SDL_Window *window, Man *man, Man *dog) {
  SDL_Event event;
  int done = 0;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_WINDOWEVENT_CLOSE:
      {
        if (window) {
          SDL_DestroyWindow(window);
          window = NULL;
          done = 1;
        }
      }
      break;
      case SDL_KEYDOWN:
      {
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            done = 1;
          break;
        }
      }
      break;
      case SDL_QUIT:
        //quit out of the game
        done = 1;
      break;
    }
  }
  
  const Uint8 *state = SDL_GetKeyboardState(NULL);
    
    if (state[SDL_SCANCODE_LEFT]) {
      dog->x -= 3;
      dog->walking = 1; 
      if (dog->x < 0) dog->x = WINDOW_WIDTH - 40;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
      dog->x += 3;
      dog->walking = 1;
      if (dog->x > WINDOW_WIDTH) dog->x = 0;
    }
    if (state[SDL_SCANCODE_UP]) {
      if (dog->y - 3 < 300){
          dog->y = 60;
      }
      else {
          dog->y -= 3;
          dog->walking = 1;
      }
    }
    if (state[SDL_SCANCODE_DOWN]) {
      if (dog->y + 3 + 60 > WINDOW_HEIGHT) {
        dog->y = WINDOW_HEIGHT - 60;
      }
      else {
        dog->y += 3;
        dog->walking = 1;
      }
    }

    if (man->x >= -40) {
        man->x += 1;
        man->walking = 1;
        if (man->x == 320) {
            man->x = -40;
        }
    }
   
     if (state[SDL_SCANCODE_SPACE]) {
      if (globalTime % 6 == 0) {
        if (man->currentSprite == 4)
          man->currentSprite = 5;
        else {
          man->currentSprite = 4;
        }
          addBullet(man->x, man-> y+70, 3);
      }
      man->shooting = 1;
    }
    else {
      man->currentSprite = 4;
      man->shooting = 0;
    }
  
  
// if(state[SDL_SCANCODE_UP] && !man->dy)
//   {
//     man->dy = -8;
//   }
//   if(state[SDL_SCANCODE_DOWN])
//   {
//     //man->y += 10;
//   }
  
  return done;
}

void doRender(SDL_Renderer *renderer, Man *man, Man *dog)
{
  //set the drawing color to blue
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  
  //Clear the screen (to blue)
  SDL_RenderClear(renderer);
  
  //set the drawing color to white
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  
  //SDL_RenderFillRect(renderer, &rect);
  SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

  //warrior
  if (man->visible) {
    SDL_Rect srcRect = { 40*man->currentSprite, 0, 40, 50 };
    SDL_Rect rect = { man->x, man->y, 40, 50 };
    SDL_RenderCopyEx(renderer, man->sheetTexture, &srcRect, &rect, 0, NULL, man->facingLeft);
  }

  //enemy
  if (dog->visible) {
     SDL_Rect eSrcRect = { 40, 0, 40, 60 };
   // SDL_Rect eSrcRect = { 40*enemy.currentSprite, 0, 40, 50 };  
    SDL_Rect eRect = { dog->x, dog->y, 40, 60 };  
    SDL_RenderCopyEx(renderer, dog->sheetTexture, &eSrcRect, &eRect, 0, NULL, dog->facingLeft);
  }
 for (int i = 0; i < MAX_BULLETS; i++) if(bullets[i]) {
    SDL_Rect rect = { bullets[i]->x, bullets[i]->y, 8, 8 };
    SDL_RenderCopy(renderer, bulletTexture, NULL, &rect);
  }
  
  //We are done drawing, "present" or show to the screen what we've drawn
  SDL_RenderPresent(renderer);
}

void updateLogic(Man *dog) {

  for(int i = 0; i < MAX_BULLETS; i++) {
    if(bullets[i]) {
      bullets[i]->y += bullets[i]->dy;
      
      //simple coll. detection
        
      if(bullets[i]->x > dog->x && bullets[i]->x < dog->x+40 &&
        bullets[i]->y > dog->y && bullets[i]->y < dog->y+50)
      {
        dog->alive = 0;           // вот тут добавляем очки а не вот это гавно
      }
      
      if(bullets[i]->x < -1000 || bullets[i]->x > 1000)
        removeBullet(i);
    }
  }
    
  globalTime++;
}

int main()
{
  SDL_Window *window;                    // Declare a window
  SDL_Renderer *renderer;                // Declare a renderer
  
  SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2
  
  Man man;
  man.x = 50;
  man.y = 0;
  man.currentSprite = 4;
  man.alive = 1;
  man.visible = 1;
  man.facingLeft = 0;
  
  Man dog;
  dog.x = 350;
  dog.y = 400;
  dog.currentSprite = 4;
  dog.facingLeft = 1;
  dog.alive = 1;
  dog.visible = 1;
  
  //Create an application window with the following settings:
  window = SDL_CreateWindow("Game Window",                     // window title
                            SDL_WINDOWPOS_UNDEFINED,           // initial x position
                            SDL_WINDOWPOS_UNDEFINED,           // initial y position
                            WINDOW_WIDTH,                               // width, in pixels
                            WINDOW_HEIGHT,                               // height, in pixels
                            0                                  // flags
                            );
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  //SDL_RenderSetLogicalSize(renderer, 320, 240);

  
  SDL_Surface *sheet = IMG_Load("sheet.png");
  if(!sheet)
  {
    printf("Cannot find sheet\n");
    return 1;
  }
  
  man.sheetTexture = SDL_CreateTextureFromSurface(renderer, sheet);
  SDL_FreeSurface(sheet);
  
  //load enemy
  sheet = IMG_Load("badman_sheet.png");
  if(!sheet)
  {
    printf("Cannot find dog sheet\n");
    return 1;
  }
  
  dog.sheetTexture = SDL_CreateTextureFromSurface(renderer, sheet);
  SDL_FreeSurface(sheet);

  //load the bg
  SDL_Surface *bg = IMG_Load("background.png");
  
  if(!sheet)
  {
    printf("Cannot find background\n");
    return 1;
  }

  backgroundTexture = SDL_CreateTextureFromSurface(renderer, bg);
  SDL_FreeSurface(bg);

  //load the bullet
  SDL_Surface *bullet = IMG_Load("bullet.png");
  
  if(!bullet)
  {
    printf("Cannot find bullet\n");
    return 1;
  }

  bulletTexture = SDL_CreateTextureFromSurface(renderer, bullet);
  SDL_FreeSurface(bullet);
  
  // The window is open: enter program loop (see SDL_PollEvent)
  int done = 0;
  
  //Event loop
  while(!done)
  {
    //Check for events
    done = processEvents(window, &man, &dog);
    
    //Update logic
    updateLogic(&dog);
    
    //Render display
    doRender(renderer, &man, &dog);
    
    //don't burn up the CPU
    SDL_Delay(10);
  }
  
  
  // Close and destroy the window
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyTexture(man.sheetTexture);
  SDL_DestroyTexture(backgroundTexture);
  SDL_DestroyTexture(bulletTexture);
  SDL_DestroyTexture(dog.sheetTexture);
  
  for(int i = 0; i < MAX_BULLETS; i++)
    removeBullet(i);
  
  // Clean up
  SDL_Quit();
  return 0;
}
