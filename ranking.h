/**
 * @file ranking.h
 * @brief Sistema de ranking – Top 10 mejores puntajes.
 *
 * Guarda, carga, ordena y muestra los mejores 10 jugadores
 * con sus nombres y puntajes en un archivo .txt.
 */

#ifndef RANKING_H_INCLUDED
#define RANKING_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "vector.h"

#define MAX_RANKING         10
#define RUTA_RANKING        "ranking.txt"
#define MAX_NOMBRE_RANKING  32

/** Entrada individual del ranking. */
typedef struct
{
    char nombre[MAX_NOMBRE_RANKING];
    int  puntaje;
} tRankingEntry;

/** Tipo puntero a función de comparación (para ordenamiento genérico). */
typedef int (*Cmp)(const void*, const void*);

/* Carga el ranking desde un archivo .txt.
 */
tVector* ranking_cargar(const char *ruta);

/*
  Guarda el ranking en un archivo .txt.*/

int ranking_guardar(const char *ruta, tVector *ranking);

/*Agrega una entrada al ranking, ordena y recorta a los mejores MAX_RANKING.

 */
void ranking_agregar(tVector *ranking, const char *nombre, int puntaje);

/*Ordenamiento por selección sobre un tVector genérico.

 */
void ranking_ordenar_seleccion(tVector *vector, Cmp cmp);

/* Renderiza el ranking sobre la pantalla SDL.

 */
void ranking_renderizar(SDL_Renderer *renderer, TTF_Font *fuenteGrande,
                        TTF_Font *fuenteChica, tVector *ranking,
                        int anchoVentana, int altoVentana);

#endif // RANKING_H_INCLUDED
