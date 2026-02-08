
#ifndef MEMORIA_H_INCLUDED
#define MEMORIA_H_INCLUDED

#include "errores.h"
#include <SDL2/SDL.h>
#include <stdint.h>

/* Estructura opaca que contiene el estado completo de una partida. */
typedef struct sMemoria tMemoria;

tMemoria* memoria_crear(SDL_Renderer *renderer, int filas, int columnas,
                        int setFiguras, int usarSonidos, int cantJugadores);

/* Libera todos los recursos de la partida. */
void memoria_destruir(tMemoria *m);

/* Procesa un evento SDL (clic y movimiento de mouse). */
tError memoria_procesar_evento(tMemoria *m, const SDL_Event *ev);

/* Actualiza la lógica (retardo tras segunda selección). */
void memoria_actualizar(tMemoria *m, uint32_t deltaMs);

/* Renderiza el tablero de cartas. */
void memoria_renderizar(tMemoria *m, SDL_Renderer *renderer);

/* Estadísticas totales (suma de ambos jugadores si hay 2). */
void memoria_obtener_estadisticas(tMemoria *m, int *puntos, int *aciertos,
                                  int *intentos, int *racha);

/* Estadísticas de un jugador específico (0 o 1). */
void memoria_obtener_estadisticas_jugador(tMemoria *m, int jugador,
                                          int *puntos, int *aciertos,
                                          int *intentos, int *racha);

/* Devuelve el turno actual (0 o 1). */
int memoria_obtener_turno(tMemoria *m);

/* Devuelve 1 si todas las parejas fueron encontradas. */
int memoria_partida_terminada(tMemoria *m);

#endif // MEMORIA_H_INCLUDED
