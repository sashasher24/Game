#include "header.h"

int done;

SDL_Texture *bulletTexture;
SDL_Texture *backgroundTexture;
Bullet *bullets[MAX_BULLETS] = { NULL };

int globalTime = 0;

static void button_process_event(button_t *btn, const SDL_Event *ev) {
    // react on mouse click within button rectangle by setting 'pressed'
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
    // draw button
    SDL_SetRenderDrawColor(r, btn->colour.r, btn->colour.g, btn->colour.b, btn->colour.a);
    SDL_RenderFillRect(r, &btn->draw_rect);

    // if button press detected - reset it so it wouldn't trigger twice
    if(btn->pressed) {
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
      if (man->x == WINDOW_WIDTH + 40){
          man->x = -40;
      }
    }

    return done;
}

void doRender(SDL_Renderer *renderer, Man *man, Man *dog) {
  //set the drawing color to blue
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  
  //Clear the screen (to blue)
  SDL_RenderClear(renderer);
  
  //set the drawing color to white
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  
  //SDL_RenderFillRect(renderer, &rect);
  SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

  //warrior
    SDL_Rect rect = { man->x, man->y, 40, 50 };
    SDL_RenderCopy(renderer, man->sheetTexture, NULL, &rect);

  //enemy
  
    SDL_Rect eRect = { dog->x, dog->y, 40, 60 };  
    SDL_RenderCopy(renderer, dog->sheetTexture, NULL, &eRect);

 for (int i = 0; i < MAX_BULLETS; i++) if(bullets[i]) {
    SDL_Rect rect = { bullets[i]->x, bullets[i]->y, 8, 8 };
    SDL_RenderCopy(renderer, bulletTexture, NULL, &rect);
  }
  
  //We are done drawing, "present" or show to the screen what we've drawn
  SDL_RenderPresent(renderer);
}

void updateLogic(Man *dog) {	
//	int k = 0;
//	int count;
  for(int i = 0; i < MAX_BULLETS; i++) {
    if(bullets[i]) {
      bullets[i]->y += bullets[i]->dy;
      
      //simple coll. detection
        
      if(bullets[i]->x >= dog->x && bullets[i]->x <= dog->x+40 &&
       bullets[i]->y >= dog->y && bullets[i]->y <= dog->y+60) {
        done = 1;
	printf("Game Over\n");
      }

      
      if(bullets[i]->x == dog->x || bullets[i]->x > 1000)
        removeBullet(i);
    }
  //  printf("%d", count);
  }
    
  globalTime++;
}

int main()
{
int quit = 0;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window1 = NULL;
    window1 = SDL_CreateWindow("", 200, 150, 200, 200, SDL_WINDOW_SHOWN);
    if (window1 == NULL){
        fprintf(stderr, "create window failed: %s\n", SDL_GetError());
        return 1;   // 'error' return status is !0. 1 is good enough
    }

    SDL_Renderer *renderer1 = NULL;
    renderer1 = SDL_CreateRenderer(window1, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer1) {   // renderer creation may fail too
        fprintf(stderr, "create renderer failed: %s\n", SDL_GetError());
        return 1;
    }
 
   // SDL_Texture* txt = NULL;

    SDL_Rect rct;
    rct.x = 0 ;
    rct.y = 0;
    rct.h = 600;
    rct.w = 600;

    // button state - colour and rectangle
    button_t start_button = {
        .colour = { .r = 255, .g = 255, .b = 255, .a = 255, },
        .draw_rect = { .x = 10, .y = 10, .w = 300, .h = 300 },
    };

    enum {
        STATE_IN_MENU,
        STATE_IN_GAME,
    } state = 0;

    while(!quit) {
        SDL_Event evt;    // no need for new/delete, stack is fine
	if (done == 1) quit = 1;
        // event loop and draw loop are separate things, don't mix them
        while(SDL_PollEvent(&evt)) {
            // quit on close, window close, or 'escape' key hit
            if(evt.type == SDL_QUIT ||
                    (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_CLOSE) ||
                    (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE)) {
                quit = 1;
            }

            // pass event to button
            button_process_event(&start_button, &evt);
        }

         SDL_SetRenderDrawColor(renderer1, 255,0, 0, 255);
        SDL_RenderClear(renderer1);

        SDL_RenderPresent(renderer1);

//      SDL_RenderCopy(renderer1, txt, NULL, &rct);
 if(state == STATE_IN_MENU) {
            //    printf("start button pressed\n");
                state = STATE_IN_GAME;   // state change - button will not be drawn anymore
            
        } else if(state == STATE_IN_GAME) {
          if(button(renderer1, &start_button)) {
            
SDL_DestroyWindow(window1);
SDL_DestroyRenderer(renderer1);
            /* your game logic */
          
   
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
  sheet = IMG_Load("dog.png");
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
//  int done = 0;
  done = 0;

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

}
}
  for(int i = 0; i < MAX_BULLETS; i++)
    removeBullet(i);
  }
  // Clean up
  SDL_Quit();
  return 0;
}
