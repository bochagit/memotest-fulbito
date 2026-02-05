#ifndef APP_H_INCLUDED
#define APP_H_INCLUDED

#include <SDL2/SDL.h>
#include "states.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    AppState state;
    int running;
} App;

int appInit(App *app);
void appHandleEvents(App *app);
void appUpdate(App *app);
void appRender(App *app);
void appCleanup(App *app);

#endif // APP_H_INCLUDED
