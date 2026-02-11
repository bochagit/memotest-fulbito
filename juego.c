/**
 * @file juego.c
 * @brief Implementación del módulo principal del juego de la memoria.
 *
 * Flujo: SDL_Init → presentación → menú → crear partida → loop → destruir.
 */

#include "juego.h"
#include "highscores.h"
#include "graficos.h"
#include "texto.h"
#include "imagenes.h"
#include "presentacion.h"
#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* ============================================================
   FUNCIONES PÚBLICAS
   ============================================================ */

tError juego_inicializar(tJuego *juego)
{
    /* ---- SDL base ---- */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "%s\n", SDL_GetError());
        return ERR_SDL;
    }

    memset(juego, 0, sizeof(tJuego));
    juego->anchoVentana = ANCHO_VENTANA;
    juego->altoVentana  = ALTO_VENTANA;

    juego->ventana = SDL_CreateWindow("Juego de la Memoria",
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     juego->anchoVentana,
                                     juego->altoVentana,
                                     SDL_WINDOW_SHOWN);
    if (!juego->ventana) { fprintf(stderr, "%s\n", SDL_GetError()); return ERR_SDL; }

    juego->renderer = SDL_CreateRenderer(juego->ventana, -1, SDL_RENDERER_ACCELERATED);
    if (!juego->renderer) { fprintf(stderr, "%s\n", SDL_GetError()); return ERR_SDL; }
    SDL_SetRenderDrawBlendMode(juego->renderer, SDL_BLENDMODE_BLEND);

    /* ---- Audio ---- */
    tFormatosSnd fmtSnd = sonidos_inicializar();
    if (fmtSnd != SONIDO_ERR) {
        juego->audioInicializado = 1;
        juego->melodia = sonidos_cargar("snd/melodia2.mp3");
        if (!juego->melodia) {
            fprintf(stderr, "Aviso: melodia.mp3 no encontrada\n");
        }
    } else {
        juego->audioInicializado = 0;
    }

    /* ---- TTF ---- */
    tError err;
    if ((err = texto_inicializar()) != TODO_OK) return err;
    juego->fuenteGrande = texto_cargar_fuente("fnt/IBMPlexMono-Regular.ttf", 48);
    juego->fuenteChica  = texto_cargar_fuente("fnt/IBMPlexMono-Regular.ttf", 24);

    /* ---- SDL_image ---- */
    if ((err = imagenes_inicializar()) != (IMAGEN_BMP | IMAGEN_JPG | IMAGEN_PNG)) {
        fprintf(stderr, "Aviso: no se pudieron cargar todos los formatos de imagen\n");
    }

    juego->fondo = imagenes_cargar_gpu(juego->renderer, "img/background.jpg");

    /* ---- Framebuffers ---- */
    for (int i = 0; i < FB_CANT; ++i)
        juego->framebuffers[i] = graficos_crear_framebuffer(juego->renderer,
                                    juego->anchoVentana, juego->altoVentana);

    /* ---- Pantalla de presentación (pide nombre jugador 1) ---- */
    juego->nombreJugador1[0] = '\0';
    presentacion_mostrar(juego->renderer, juego->fuenteGrande,
                         "img/fondo_presentacion.png",
                         "snd/Sonido_presentacion.mp3",
                         juego->nombreJugador1,
                         sizeof(juego->nombreJugador1));

    /* ---- Cargar configuración persistente ---- */
    juego->configuracion = config_cargar(RUTA_CONFIG);

    /* ---- Menú gráfico ---- */
    juego->nombreJugador2[0] = '\0';
/* ---- Menú gráfico (Navegación con Highscores) ---- */
    juego->nombreJugador2[0] = '\0';
    int navegando = 1;

    while (navegando) {
        tAccionMenu accion = menu_mostrar(juego->renderer,
                                          juego->fuenteChica,
                                          "img/fondo_presentacion.png",
                                          &juego->configuracion,
                                          juego->nombreJugador2,
                                          sizeof(juego->nombreJugador2));

        if (accion == ACCION_VER_SCORES) {
            // Mostramos la tabla y, al salir de esta función, el bucle repite el menú
            menu_mostrar_highscores(juego->renderer, juego->fuenteChica, "img/fondo_presentacion.png");
        }
        else if (accion == ACCION_JUGAR) {
            // Salimos del bucle para continuar con la creación de la partida abajo
            navegando = 0;
        }
        else {
            // Si cerró la ventana o hubo error (ACCION_SALIR)
            juego->corriendo = 0;
            return ERR_SDL;
        }
    }


    /* Guardar configuración para la próxima sesión */
    config_guardar(RUTA_CONFIG, &juego->configuracion);

    /* ---- Crear partida de memoria ---- */
    srand((unsigned)time(NULL));
    juego->partida = memoria_crear(juego->renderer,
                                   juego->configuracion.filas,
                                   juego->configuracion.columnas,
                                   juego->configuracion.setFiguras,
                                   juego->audioInicializado,
                                   juego->configuracion.cantJugadores);
    if (!juego->partida) {
        fprintf(stderr, "Error al crear la partida de memoria.\n");
        return ERR_MEMORIA;
    }

    /* ---- Música de fondo ---- */
    if (juego->audioInicializado && juego->melodia)
        sonidos_reproducir(juego->melodia, -1);

    juego->corriendo = 1;
    return TODO_OK;
}


tError juego_procesar_eventos(tJuego *juego)
{
    tError err = TODO_OK;
    SDL_Event evento;

    while (SDL_PollEvent(&evento)) {
        if (evento.type == SDL_QUIT) {
            juego->corriendo = 0;
        }

        /* Enviar todos los eventos relevantes al módulo de memoria */
        if (juego->partida) {
            if (evento.type == SDL_MOUSEBUTTONDOWN ||
                evento.type == SDL_MOUSEMOTION) {
                memoria_procesar_evento(juego->partida, &evento);
            }
        }

        /* ESC para salir */
        if (evento.type == SDL_KEYDOWN && evento.key.keysym.sym == SDLK_ESCAPE) {
            juego->corriendo = 0;
        }
    }
    return err;
}


void juego_actualizar(tJuego *juego)
{
    static uint32_t ticksPrev = 0;
    uint32_t ticks = SDL_GetTicks();
    uint32_t delta = ticksPrev ? (ticks - ticksPrev) : LOOP_DELAY;
    ticksPrev = ticks;

    if (juego->partida)
        memoria_actualizar(juego->partida, delta);
}


void juego_renderizar(tJuego *juego)
{
    /* ---- Capa: fondo ---- */
    graficos_cambiar_framebuffer(juego->renderer, juego->framebuffers[FB_FONDO]);
    graficos_borrar_pantalla(juego->renderer, &(SDL_Color){0,0,0,255});
    if (juego->fondo) {
        SDL_RenderCopy(juego->renderer, juego->fondo, NULL, NULL);
    }

    /* ---- Capa: escena (tablero de cartas) ---- */
    graficos_cambiar_framebuffer(juego->renderer, juego->framebuffers[FB_ESCENA]);
    graficos_borrar_pantalla(juego->renderer, &(SDL_Color){0,0,0,0});
    if (juego->partida)
        memoria_renderizar(juego->partida, juego->renderer);

    /* ---- Capa: HUD (estadísticas y nombres) ---- */
    graficos_cambiar_framebuffer(juego->renderer, juego->framebuffers[FB_HUD]);
    graficos_borrar_pantalla(juego->renderer, &(SDL_Color){0,0,0,0});

    if (juego->fuenteChica && juego->partida) {
        SDL_Color blanco  = {255,255,255,255};
        SDL_Color amarillo = {255,255,100,255};
        int terminada = memoria_partida_terminada(juego->partida);

        if (juego->configuracion.cantJugadores == 2) {
            /* ---- Modo 2 jugadores ---- */
            int turno = memoria_obtener_turno(juego->partida);
            for (int j = 0; j < 2; ++j) {
                int pts = 0, ac = 0, it = 0, rac = 0;
                memoria_obtener_estadisticas_jugador(juego->partida, j, &pts, &ac, &it, &rac);
                const char *nombre = (j == 0) ? juego->nombreJugador1 : juego->nombreJugador2;
                if (!nombre[0]) nombre = (j == 0) ? "Jugador 1" : "Jugador 2";
                char linea[256];
                snprintf(linea, sizeof(linea), "%s%s  Pts:%d  Ac:%d  Int:%d  Racha:%d",
                         (turno == j && !terminada) ? ">> " : "   ",
                         nombre, pts, ac, it, rac);
                SDL_Texture *tLinea = texto_crear_textura(juego->renderer,
                    juego->fuenteChica, linea, (turno == j) ? amarillo : blanco);
                if (tLinea) {
                    int w, h; SDL_QueryTexture(tLinea, NULL, NULL, &w, &h);
                    SDL_Rect dst = { 10, 10 + j*30, w, h };
                    SDL_RenderCopy(juego->renderer, tLinea, NULL, &dst);
                    SDL_DestroyTexture(tLinea);
                }
            }
        } else {
            /* ---- Modo 1 jugador ---- */
            int pts = 0, ac = 0, it = 0, rac = 0;
            memoria_obtener_estadisticas(juego->partida, &pts, &ac, &it, &rac);
            const char *nombre = juego->nombreJugador1[0] ? juego->nombreJugador1 : "Jugador";
            char linea[256];
            snprintf(linea, sizeof(linea), "%s  Pts:%d  Aciertos:%d  Intentos:%d  Racha:%d",
                     nombre, pts, ac, it, rac);
            SDL_Texture *tLinea = texto_crear_textura(juego->renderer,
                juego->fuenteChica, linea, blanco);
            if (tLinea) {
                int w, h; SDL_QueryTexture(tLinea, NULL, NULL, &w, &h);
                SDL_Rect dst = { 10, 10, w, h };
                SDL_RenderCopy(juego->renderer, tLinea, NULL, &dst);
                SDL_DestroyTexture(tLinea);
            }
        }

        /* Mensaje de fin de partida */
        if (terminada) {
            if (!juego->highscoreGuardado) {
            int ptsFinales = 0;
            // Obtenemos los puntos del jugador 1 (o el total)
            memoria_obtener_estadisticas(juego->partida, &ptsFinales, NULL, NULL, NULL);

            const char *nombre = juego->nombreJugador1[0] ? juego->nombreJugador1 : "Jugador";

            // USAMOS EL NOMBRE CORRECTO DE TU FUNCIÓN:
            highscores_insertar(nombre, ptsFinales,juego->configuracion.filas,juego->configuracion.columnas);

            juego->highscoreGuardado = 1; // Para que no guarde mil veces por segundo
            printf("Puntaje de %s (%d pts) guardado en el ranking.\n", nombre, ptsFinales);
            }

            const char *msgFin = "Partida terminada! (ESC para salir)";
            if (juego->configuracion.cantJugadores == 2) {
                int p0=0, p1=0;
                memoria_obtener_estadisticas_jugador(juego->partida, 0, &p0, NULL, NULL, NULL);
                memoria_obtener_estadisticas_jugador(juego->partida, 1, &p1, NULL, NULL, NULL);
                if (p0 > p1) msgFin = "Gana Jugador 1! (ESC para salir)";
                else if (p1 > p0) msgFin = "Gana Jugador 2! (ESC para salir)";
                else msgFin = "Empate! (ESC para salir)";
            }
            SDL_Texture *tFin = texto_crear_textura(juego->renderer,
                juego->fuenteGrande, msgFin, (SDL_Color){255,200,50,255});
            if (tFin) {
                int w, h; SDL_QueryTexture(tFin, NULL, NULL, &w, &h);
                SDL_Rect dst = { (int)(juego->anchoVentana/2 - w/2),
                                 (int)(juego->altoVentana/2 - h/2), w, h };
                /* Fondo oscuro detrás del texto */
                SDL_Rect bg = { dst.x - 20, dst.y - 10, dst.w + 40, dst.h + 20 };
                SDL_SetRenderDrawBlendMode(juego->renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(juego->renderer, 0, 0, 0, 180);
                SDL_RenderFillRect(juego->renderer, &bg);
                SDL_RenderCopy(juego->renderer, tFin, NULL, &dst);
                SDL_DestroyTexture(tFin);
            }
        }
    }

    /* ---- Composición final ---- */
    graficos_cambiar_framebuffer(juego->renderer, NULL);
    graficos_renderizar(juego->renderer, juego->framebuffers, FB_CANT);
}


void juego_destruir(tJuego *juego)
{
    if (juego->partida) {
        memoria_destruir(juego->partida);
        juego->partida = NULL;
    }

    for (int i = 0; i < FB_CANT; ++i) {
        if (juego->framebuffers[i])
            SDL_DestroyTexture(juego->framebuffers[i]);
    }

    if (juego->fuenteGrande) texto_destruir_fuente(juego->fuenteGrande);
    if (juego->fuenteChica)  texto_destruir_fuente(juego->fuenteChica);
    texto_finalizar();

    if (juego->audioInicializado) {
        if (juego->melodia) sonidos_destruir(juego->melodia);
        sonidos_finalizar();
    }

    if (juego->fondo) SDL_DestroyTexture(juego->fondo);
    imagenes_finalizar();

    if (juego->renderer) SDL_DestroyRenderer(juego->renderer);
    if (juego->ventana)  SDL_DestroyWindow(juego->ventana);

    SDL_Quit();
}
