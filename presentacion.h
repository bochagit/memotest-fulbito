#ifndef PRESENTACION_H_INCLUDED
#define PRESENTACION_H_INCLUDED

#include "errores.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/*
  Muestra la pantalla de presentación usando la textura de fondo y reproduce un sonido.

 */
tError presentacion_mostrar(SDL_Renderer *renderer, TTF_Font *fuente, const char *fondoPath, const char *sonidoPath, const char *mensaje, char *outName, size_t maxLen);

#endif // PRESENTACION_H_INCLUDED
