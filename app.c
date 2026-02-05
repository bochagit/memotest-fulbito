#include "app.h"
#include <stdio.h>

int appInit(App *app){
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
        printf("Error SDL_Init: %s\n", SDL_GetError());
        return 0;
    }

    app->window = SDL_CreateWindow(
        "Memotest - Base SDL",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1024, 768,
        SDL_WINDOW_SHOWN
    );

    if (!app->window){
        printf("Error creando ventana: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    app->renderer = SDL_CreateRenderer(
        app->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!app->renderer){
        SDL_DestroyWindow(app->window);
        SDL_Quit();
        return 0;
    }

    app->state = STATE_MENU;
    app->running = 1;

    return 1;
}

void appHandleEvents(App *app){
    SDL_Event e;
    while (SDL_PollEvent(&e)){
        if (e.type == SDL_QUIT){
            app->state = STATE_EXIT;
        }

        if (e.type == SDL_KEYDOWN){
            if (e.key.keysym.sym == SDLK_ESCAPE){
                app->state = STATE_EXIT;
            }

            if (e.key.keysym.sym == SDLK_RETURN){
                if (app->state == STATE_MENU){
                    app->state = STATE_GAME;
                }
            }
        }
    }
}

void appUpdate(App *app){
    if (app->state == STATE_EXIT){
        app->running = 0;
    }
}

void appRender(App *app){
    switch (app->state){
        case STATE_MENU:
            SDL_SetRenderDrawColor(app->renderer, 30, 30, 30, 255);
            break;

        case STATE_GAME:
            SDL_SetRenderDrawColor(app->renderer, 0, 120, 0, 255);
            break;

        default:
            SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
            break;
    }

    SDL_RenderClear(app->renderer);
    SDL_RenderPresent(app->renderer);
}

void appCleanup(App *app){
    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
    SDL_Quit();
}
