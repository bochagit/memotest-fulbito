
#ifndef JUEGO_H_INCLUDED
#define JUEGO_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdint.h>
#include "sonidos.h"
#include "memoria.h"
#include "config.h"

#define LOOP_DELAY      16
#define ANCHO_VENTANA  1024
#define ALTO_VENTANA    768
#define RUTA_CONFIG    "config.cfg"

/* Capas de renderizado */
typedef enum {
    FB_FONDO,
    FB_ESCENA,
    FB_HUD,
    FB_CANT,
} eFramebuffers;

/* Estructura principal del juego */
typedef struct {
    SDL_Window   *ventana;
    SDL_Renderer *renderer;
    SDL_Texture  *framebuffers[FB_CANT];
    SDL_Texture  *fondo;               /* textura de fondo */
    TTF_Font     *fuenteGrande;        /* tamaño 48 – títulos */
    TTF_Font     *fuenteChica;         /* tamaño 24 – stats/menú */
    tSonido      *melodia;             /* música de fondo */
    uint32_t      anchoVentana;
    uint32_t      altoVentana;
    uint8_t       audioInicializado;
    uint8_t       corriendo;
    char          nombreJugador1[32];
    char          nombreJugador2[32];
    tConfig       configuracion;
    tMemoria     *partida;
    uint8_t highscoreGuardado;
} tJuego;

tError juego_inicializar(tJuego *juego);
tError juego_procesar_eventos(tJuego *juego);
void   juego_actualizar(tJuego *juego);
void   juego_renderizar(tJuego *juego);
void   juego_destruir(tJuego *juego);

#endif // JUEGO_H_INCLUDED
