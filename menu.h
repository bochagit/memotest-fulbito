#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "config.h"
#include "errores.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Definimos qué acción eligió el usuario en el menú
typedef enum {
    ACCION_JUGAR,
    ACCION_VER_SCORES,
    ACCION_SALIR
} tAccionMenu;

// Ahora la función devuelve la acción elegida
tAccionMenu menu_mostrar(SDL_Renderer *renderer, TTF_Font *fuente, const char *fondoPath, tConfig *cfg, char *nombreJugador2, size_t maxLen);

// Nueva función para mostrar la tabla
void menu_mostrar_highscores(SDL_Renderer *renderer, TTF_Font *fuente, const char *fondoPath);

#endif
