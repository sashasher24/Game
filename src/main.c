#include "header.h"

int done;

SDL_Texture *bulletTexture;
SDL_Texture *backgroundTexture;
Bullet *bullets[MAX_BULLETS] = { NULL };

int globalTime = 0;

static void button_process_event(button_t *btn, const SDL_Event *ev) {
  if(ev->type == SDL_MOUSEBUTTONDOWN) {
    if(ev->button.button == SDL_BUTTON_LEFT &&
        ev->button.x >= btn->draw_rect.x &&
        ev->button.x <= (btn->draw_rect.x + btn->draw_rect.w) &&
        ev->button.y >= btn->draw_rect.y &&
        ev->button.y <= (btn->draw_rect.y + btn->draw_rect.h)) {
            btn->pressed = TRUE;
    }
  }
}

static bool button(SDL_Renderer *r, button_t *btn) {
  SDL_SetRenderDrawColor(r, btn->colour.r, btn->colour.g, btn->colour.b, btn->colour.a);
  SDL_RenderFillRect(r, &btn->draw_rect);

  if (btn->pressed) {
    btn->pressed = FALSE;
    return TRUE;
  }
  return FALSE;
}

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
  done = 0;

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
      dog->y = 300;
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
  
    int k = rand() % 320;
    int m = rand() % 320;
    int h = rand() % 100;
    int t = rand() % 50;
      
    if (man->x == k || man->x == m || man->x == h || man->x == t ) {
      addBullet(man->x, man-> y+70, 3);
      man->shooting = 1;
    }
    if (man->x == WINDOW_WIDTH + 40) {
      man->x = -40;
    }
  }
  return done;
}

void doRender(SDL_Renderer *renderer, Man *man, Man *dog) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  
  SDL_RenderClear(renderer);
  
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  
  SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

  SDL_Rect rect = { man->x, man->y, 40, 50 };
  SDL_RenderCopy(renderer, man->sheetTexture, NULL, &rect);

  SDL_Rect eRect = { dog->x, dog->y, 40, 60 };  
  SDL_RenderCopy(renderer, dog->sheetTexture, NULL, &eRect);

  for (int i = 0; i < MAX_BULLETS; i++) if(bullets[i]) {
    SDL_Rect rect = { bullets[i]->x, bullets[i]->y, 8, 8 };
    SDL_RenderCopy(renderer, bulletTexture, NULL, &rect);
  }
  
  SDL_RenderPresent(renderer);
}

void updateLogic(Man *dog) {	
  for(int i = 0; i < MAX_BULLETS; i++) {
    if(bullets[i]) {
      bullets[i]->y += bullets[i]->dy; 
      if(bullets[i]->x >= dog->x && bullets[i]->x <= dog->x+40 &&
            bullets[i]->y >= dog->y && bullets[i]->y <= dog->y+60) {
        done = 1;
	      printf("Game Over\n");
      }
      if(bullets[i]->x == dog->x || bullets[i]->x > 1000) {
        removeBullet(i);
      }
    }
  }
  globalTime++;
}

int main() {
  int quit = 0;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window1 = NULL;
  window1 = SDL_CreateWindow("", 200, 150, 200, 200, SDL_WINDOW_SHOWN);
  if (window1 == NULL){
    fprintf(stderr, "create window failed: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Renderer *renderer1 = NULL;
  renderer1 = SDL_CreateRenderer(window1, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer1) {
    fprintf(stderr, "create renderer failed: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Rect rct;
  rct.x = 0 ;
  rct.y = 0;
  rct.h = 600;
  rct.w = 600;

  button_t start_button = {
    .colour = { .r = 255, .g = 255, .b = 255, .a = 255, },
    .draw_rect = { .x = 10, .y = 10, .w = 300, .h = 300 },
  };

  enum {
      STATE_IN_MENU,
      STATE_IN_GAME,
  } state = 0;

  while(!quit) {
    SDL_Event evt;
	  if (done == 1) quit = 1;
    while (SDL_PollEvent(&evt)) {
      if (evt.type == SDL_QUIT ||
          (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_CLOSE) ||
          (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE)) {
        quit = 1;
      }
      button_process_event(&start_button, &evt);
    }

    SDL_SetRenderDrawColor(renderer1, 255,0, 0, 255);
    SDL_RenderClear(renderer1);
    SDL_RenderPresent(renderer1);
    if (state == STATE_IN_MENU) {
      state = STATE_IN_GAME;
            
    } else if (state == STATE_IN_GAME) {
        if(button(renderer1, &start_button)) {
            
          SDL_DestroyWindow(window1);
          SDL_DestroyRenderer(renderer1);          
   
          SDL_Window *window;
          SDL_Renderer *renderer;
          
          SDL_Init(SDL_INIT_VIDEO);
          
          Man man;
          man.x = 50;
          man.y = 0;
          
          Man dog;
          dog.x = 350;
          dog.y = 400;
  
          window = SDL_CreateWindow("Game Window",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    WINDOW_WIDTH,
                                    WINDOW_HEIGHT,
                                    0);
          renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  
          SDL_Surface *sheet = IMG_Load("sheet.png");
          if (!sheet) {
            printf("Cannot find sheet\n");
            return 1;
          }
          man.sheetTexture = SDL_CreateTextureFromSurface(renderer, sheet);
          SDL_FreeSurface(sheet);
  
          sheet = IMG_Load("dog.png");
          if (!sheet) {
            printf("Cannot find dog sheet\n");
            return 1;
          }
          dog.sheetTexture = SDL_CreateTextureFromSurface(renderer, sheet);
          SDL_FreeSurface(sheet);

          SDL_Surface *bg = IMG_Load("background.png");
          if (!sheet) {
            printf("Cannot find background\n");
            return 1;
          }
          backgroundTexture = SDL_CreateTextureFromSurface(renderer, bg);
          SDL_FreeSurface(bg);

          SDL_Surface *bullet = IMG_Load("bullet.png");
          if (!bullet) {
            printf("Cannot find bullet\n");
            return 1;
          }
          bulletTexture = SDL_CreateTextureFromSurface(renderer, bullet);
          SDL_FreeSurface(bullet);
          done = 0;

          while (!done) {
            done = processEvents(window, &man, &dog);
            updateLogic(&dog);
            doRender(renderer, &man, &dog);
            SDL_Delay(10);
          }
  
          SDL_DestroyWindow(window);
          SDL_DestroyRenderer(renderer);
          SDL_DestroyTexture(man.sheetTexture);
          SDL_DestroyTexture(backgroundTexture);
          SDL_DestroyTexture(bulletTexture);
          SDL_DestroyTexture(dog.sheetTexture);

        }
      }
    for (int i = 0; i < MAX_BULLETS; i++) {
      removeBullet(i);
    }
  }
  SDL_Quit();
  return 0;
}
