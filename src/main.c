#include "header.h"

int done;

SDL_Texture *objectTexture;
SDL_Texture *backgroundTexture;
Object *objects[MAX_BULLETS] = { NULL };

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

static bool button(button_t *btn) {
   if (btn->pressed) {
    btn->pressed = FALSE;
    return TRUE;
  }
  return FALSE;
}

void addBullet(float x, float y, float dy) {
  int found = -1;
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (objects[i] == NULL) {
      found = i;
      break;
    }
  }

  if (found >= 0) {
    int i = found;
    objects[i] = malloc(sizeof(Object));
    objects[i]->x = x;
    objects[i]->y = y;
    objects[i]->dy = dy;
  }
}

void removeBullet(int i) {
  if (objects[i]) {
    free(objects[i]);
    objects[i] = NULL;
  }
}

int processEvents(SDL_Window *window, Rocket *rocket, Rocket *dog) {
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
    dog->x -= 2;
    if (dog->x - 2 < 0) dog->x = 0;
  }
  if (state[SDL_SCANCODE_RIGHT]) {
    if (dog->x + 2 > WINDOW_WIDTH - 60) dog->x = WINDOW_WIDTH - 60;
    dog->x += 2;
  }
  if (state[SDL_SCANCODE_UP]) {
    if (dog->y - 2 < 405) {
      dog->y = 405;
    }
    else {
      dog->y -= 2;
    }
  }
  if (state[SDL_SCANCODE_DOWN]) {
    if (dog->y + 2 + 40 > WINDOW_HEIGHT) {
      dog->y = WINDOW_HEIGHT - 40;
    }
    else {
      dog->y += 2;
    }
  }
    
  if (rocket->x >= -60) {
    rocket->x += 2;

    int a = rand() % 800;
    if (a % 13 == 0) {
	addBullet(rocket->x, rocket->y + 70, 3);
	rocket->shooting = 1;
    }
    if (rocket->x == WINDOW_WIDTH + 60) {
	rocket->x = -60;
    }
  }
  return done;
}

void doRender(SDL_Renderer *renderer, Rocket *rocket, Rocket *dog) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  
  SDL_RenderClear(renderer);
  
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  
  SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

  SDL_Rect rect = { rocket->x, rocket->y, 90, 70 };
  SDL_RenderCopy(renderer, rocket->sheetTexture, NULL, &rect);

  SDL_Rect eRect = { dog->x, dog->y, 60, 40 };  
  SDL_RenderCopy(renderer, dog->sheetTexture, NULL, &eRect);

  for (int i = 0; i < MAX_BULLETS; i++) if(objects[i]) {
    SDL_Rect rect = { objects[i]->x, objects[i]->y, 20, 20};
    SDL_RenderCopy(renderer, objectTexture, NULL, &rect);
  }
  
  SDL_RenderPresent(renderer);
}

void updateLogic(Rocket *dog) {	
  for(int i = 0; i < MAX_BULLETS; i++) {
    if(objects[i]) {
      objects[i]->y += objects[i]->dy; 
      if(objects[i]->x >= dog->x && objects[i]->x <= dog->x+40 &&
            objects[i]->y >= dog->y && objects[i]->y <= dog->y+60) {
//	done = 1;
	SDL_Window *window_end = SDL_CreateWindow("game over",
                        SDL_WINDOWPOS_UNDEFINED,
                        SDL_WINDOWPOS_UNDEFINED,
                        500,
                        500,
                        SDL_WINDOW_SHOWN);
	if (window_end == NULL) {
	    fprintf(stderr, "create window failed: %s\n", SDL_GetError());
	    exit(1);
	}
	SDL_Renderer *renderer_end = SDL_CreateRenderer(window_end, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer_end) {
	    fprintf(stderr, "create renderer failed: %s\n", SDL_GetError());
    	    exit(1);
	}
	SDL_Surface *end_surface = IMG_Load("./resource/game_ov.jpg");
	if (!end_surface) {
	    printf("cannot find picture\n");
    	    exit(1);
	}
	SDL_Texture *end_texture = SDL_CreateTextureFromSurface(renderer_end, end_surface);
	SDL_FreeSurface(end_surface);
	SDL_RenderCopy(renderer_end, end_texture, NULL, NULL);
	SDL_RenderPresent(renderer_end);
//	done = 1;
	done = 2;
      }
      if(objects[i]->x == dog->x || objects[i]->x > 1000) {
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
  window1 = SDL_CreateWindow("start",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			500,
			500,
			SDL_WINDOW_SHOWN);
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
  SDL_Surface *menuSurface = IMG_Load("./resource/start_page.jpg");
  if (!menuSurface) {
    printf("cannot find menu\n");
    return 1;
  }
  SDL_Texture *menu = SDL_CreateTextureFromSurface(renderer1, menuSurface);
  SDL_FreeSurface(menuSurface);

  SDL_Rect rct;
  rct.x = 0 ;
  rct.y = 0;
  rct.h = 600;
  rct.w = 600;

  button_t start_button = {
    .draw_rect = { .x = 0, .y = 270, .w = 500, .h = 230 },
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

    SDL_RenderCopy(renderer1, menu, NULL, NULL);
    SDL_RenderPresent(renderer1);
    if (state == STATE_IN_MENU) {
      state = STATE_IN_GAME;
            
    } else if (state == STATE_IN_GAME) {
        if(button(&start_button)) {
            
          SDL_DestroyWindow(window1);
          SDL_DestroyRenderer(renderer1);          
   
          SDL_Window *window;
          SDL_Renderer *renderer;
          
          SDL_Init(SDL_INIT_VIDEO);
          
          Rocket rocket;
          rocket.x = 50;
          rocket.y = 0;
          
          Rocket dog;
          dog.x = 350;
          dog.y = 420;
  
          window = SDL_CreateWindow("Our Game",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    WINDOW_WIDTH,
                                    WINDOW_HEIGHT,
                                    0);
          renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  
          SDL_Surface *sheet = IMG_Load("./resource/reketka.jpg");
          if (!sheet) {
            printf("Cannot find sheet\n");
            return 1;
          }
          rocket.sheetTexture = SDL_CreateTextureFromSurface(renderer, sheet);
          SDL_FreeSurface(sheet);
  
          sheet = IMG_Load("./resource/dog_UP.jpg");
          if (!sheet) {
            printf("Cannot find dog sheet\n");
            return 1;
          }
          dog.sheetTexture = SDL_CreateTextureFromSurface(renderer, sheet);
          SDL_FreeSurface(sheet);

          SDL_Surface *bg = IMG_Load("./resource/back_game.jpg");
          if (!sheet) {
            printf("Cannot find background\n");
            return 1;
          }
          backgroundTexture = SDL_CreateTextureFromSurface(renderer, bg);
          SDL_FreeSurface(bg);

          SDL_Surface *object = IMG_Load("./resource/virus.jpeg");
          if (!object) {
            printf("Cannot find object\n");
            return 1;
          }
          objectTexture = SDL_CreateTextureFromSurface(renderer, object);
          SDL_FreeSurface(object);
          done = 0;

          while (!done) {
	    if (done == 2) done = 1;
            done = processEvents(window, &rocket, &dog);
            updateLogic(&dog);
            doRender(renderer, &rocket, &dog);
            SDL_Delay(10);
          }
  
          SDL_DestroyWindow(window);
          SDL_DestroyRenderer(renderer);
          SDL_DestroyTexture(rocket.sheetTexture);
          SDL_DestroyTexture(backgroundTexture);
          SDL_DestroyTexture(objectTexture);
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
