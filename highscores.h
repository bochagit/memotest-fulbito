#ifndef HIGHSCORES_H_INCLUDED
#define HIGHSCORES_H_INCLUDED

#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define MAX_NOMBRE_HS 50
#define MAX_TOP 10
#define RUTA_HIGHSCORES "highscores.txt"

typedef struct {
    char nombre[MAX_NOMBRE_HS];
    int puntos;
    int filas;
    int columnas;
} tEntradaScore;

/**
 * @brief Carga los puntajes desde el archivo a un array.
 * @return Cantidad de puntajes leídos.
 */
int highscores_cargar(tEntradaScore scores[]);

/**
 * @brief Intenta insertar un nuevo puntaje. Si entra en el Top 10,
 * actualiza el archivo.
 */
void highscores_insertar(const char *nombre, int puntos, int filas, int columnas);


#endif

#endif // HIGHSCORES_H_INCLUDED
