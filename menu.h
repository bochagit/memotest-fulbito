
#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "config.h"
#include "errores.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/*
  Muestra el menú gráfico de configuración.
 */
tError menu_mostrar(SDL_Renderer *renderer, TTF_Font *fuente,
                    tConfig *cfg, char *nombreJugador2, size_t maxLen);

#endif // MENU_H_INCLUDED
