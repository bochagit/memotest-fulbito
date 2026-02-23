#include "juego.h"
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

    /* ---- Pedir SOLO el nombre del jugador 1 al inicio ---- */
    juego->nombreJugador1[0] = '\0';
    juego->nombreJugador2[0] = '\0';
    
    presentacion_mostrar(juego->renderer, juego->fuenteGrande,
                         "img/fondo_presentacion.png",
                         "snd/Sonido_presentacion.mp3",
                         "Ingrese su nombre",
                         juego->nombreJugador1,
                         sizeof(juego->nombreJugador1));

    /* ---- Cargar configuración persistente ---- */
    juego->configuracion = config_cargar(RUTA_CONFIG);

    /* ---- Menú gráfico (Navegación con Highscores) ---- */
    int navegando = 1;
    while (navegando) {
        tAccionMenu accion = menu_mostrar(juego->renderer,
                                          juego->fuenteChica,
                                          "img/fondo_presentacion.png",
                                          &juego->configuracion,
                                          juego->nombreJugador1,
                                          juego->nombreJugador2,
                                          sizeof(juego->nombreJugador1),
                                          sizeof(juego->nombreJugador2));

        if (accion == ACCION_VER_SCORES) {
            menu_mostrar_highscores(juego->renderer, juego->fuenteChica, "img/fondo_presentacion.png");
        }
        else if (accion == ACCION_CAMBIAR_NOMBRES) {
            presentacion_mostrar(juego->renderer, juego->fuenteGrande,
                               "img/fondo_presentacion.png",
                               "snd/Sonido_presentacion.mp3",
                               "Ingrese el nombre de jugador 1",
                               juego->nombreJugador1,
                               sizeof(juego->nombreJugador1));
            
            presentacion_mostrar(juego->renderer, juego->fuenteGrande,
                               "img/fondo_presentacion.png",
                               "snd/Sonido_presentacion.mp3",
                               "Ingrese el nombre de jugador 2",
                               juego->nombreJugador2,
                               sizeof(juego->nombreJugador2));
        }
        else if (accion == ACCION_JUGAR) {
            /* Si eligió 2 jugadores y no tiene nombre el jugador 2, pedirlo */
            if (juego->configuracion.cantJugadores == 2 && !juego->nombreJugador2[0]) {
                presentacion_mostrar(juego->renderer, juego->fuenteGrande,
                                   "img/fondo_presentacion.png",
                                   "snd/Sonido_presentacion.mp3",
                                   "Ingrese el nombre de jugador 2",
                                   juego->nombreJugador2,
                                   sizeof(juego->nombreJugador2));
            }
            navegando = 0;
        }
        else {
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

tAccionMenu juego_procesar_eventos(tJuego *juego)
{
    tAccionMenu accion = -1;   // ninguna acción por defecto
    SDL_Event evento;

    // Rectángulo del botón Cancelar (arriba a la derecha)
    SDL_Rect botonCancelar = { juego->anchoVentana - 170, 20, 150, 40 };

    while (SDL_PollEvent(&evento)) {
        // Cerrar ventana
        if (evento.type == SDL_QUIT) {
            juego->corriendo = 0;
            accion = ACCION_SALIR;
        }

        // Clic con el mouse
        else if (evento.type == SDL_MOUSEBUTTONDOWN && evento.button.button == SDL_BUTTON_LEFT) {
            int mx = evento.button.x;
            int my = evento.button.y;

            // Detectar clic dentro del botón Cancelar
            if (!memoria_partida_terminada(juego->partida) &&
                mx >= botonCancelar.x && mx <= botonCancelar.x + botonCancelar.w &&
                my >= botonCancelar.y && my <= botonCancelar.y + botonCancelar.h) {
                if(juego->partida){
                    memoria_destruir(juego->partida);
                    juego->partida = NULL;
                }
                juego->rankingGuardado = 0;
                accion = ACCION_VOLVER_MENU;   // <-- vuelve al menú
            }
        }

        // Teclas
        else if (evento.type == SDL_KEYDOWN) {
            // ESC: salir del juego
            if (evento.key.keysym.sym == SDLK_ESCAPE) {
                if (memoria_partida_terminada(juego->partida)) {
                    juego->corriendo = 0;
                    accion = ACCION_SALIR;
                } else {
                    // Durante partida, preguntar confirmación o ir directo al menú
                    if (juego->partida) {
                        memoria_destruir(juego->partida);
                        juego->partida = NULL;
                    }
                    juego->rankingGuardado = 0;
                    accion = ACCION_VOLVER_MENU;
                }
            }
            // ENTER: reiniciar partida con misma configuración
            else if (evento.key.keysym.sym == SDLK_RETURN || evento.key.keysym.sym == SDLK_KP_ENTER) {
                if (memoria_partida_terminada(juego->partida)) {
                    // Liberar partida anterior
                    if (juego->partida) {
                        memoria_destruir(juego->partida);
                        juego->partida = NULL;
                    }
                    // Liberar ranking guardado
                    if (juego->ranking) {
                        vector_destroy(juego->ranking);
                        juego->ranking = NULL;
                    }
                    juego->rankingGuardado = 0;

                    // Crear nueva partida con la misma configuración
                    juego->partida = memoria_crear(juego->renderer,
                                                   juego->configuracion.filas,
                                                   juego->configuracion.columnas,
                                                   juego->configuracion.setFiguras,
                                                   juego->audioInicializado,
                                                   juego->configuracion.cantJugadores);

                    if (!juego->partida) {
                        fprintf(stderr, "Error al reiniciar la partida.\n");
                        accion = ACCION_SALIR;
                    }
                }
            }
        }

        // Eventos de la partida (si está activa)
        if (juego->partida && !memoria_partida_terminada(juego->partida)) {
            if (evento.type == SDL_MOUSEBUTTONDOWN || evento.type == SDL_MOUSEMOTION) {
                memoria_procesar_evento(juego->partida, &evento);
            }
        }
    }

    return accion;
}

void juego_actualizar(tJuego *juego)
{
    static uint32_t ticksPrev = 0;
    uint32_t ticks = SDL_GetTicks();
    uint32_t delta = ticksPrev ? (ticks - ticksPrev) : LOOP_DELAY;
    ticksPrev = ticks;

    if (juego->partida)
        memoria_actualizar(juego->partida, delta);

    /* ---- Guardar ranking al terminar la partida (una sola vez) ---- */
    if (juego->partida && memoria_partida_terminada(juego->partida)
        && !juego->rankingGuardado)
    {
        juego->ranking = ranking_cargar(RUTA_RANKING);
        if (juego->ranking)
        {
            if (juego->configuracion.cantJugadores == 2)
            {
                int p0 = 0, p1 = 0;
                memoria_obtener_estadisticas_jugador(juego->partida, 0, &p0, NULL, NULL, NULL);
                memoria_obtener_estadisticas_jugador(juego->partida, 1, &p1, NULL, NULL, NULL);
                const char *n1 = juego->nombreJugador1[0] ? juego->nombreJugador1 : "Jugador 1";
                const char *n2 = juego->nombreJugador2[0] ? juego->nombreJugador2 : "Jugador 2";
                ranking_agregar(juego->ranking, n1, p0);
                ranking_agregar(juego->ranking, n2, p1);
            }
            else
            {
                int pts = 0;
                memoria_obtener_estadisticas(juego->partida, &pts, NULL, NULL, NULL);
                const char *nombre = juego->nombreJugador1[0] ? juego->nombreJugador1 : "Jugador";
                ranking_agregar(juego->ranking, nombre, pts);
            }
            ranking_guardar(RUTA_RANKING, juego->ranking);
        }
        juego->rankingGuardado = 1;
    }
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

        /* Pantalla de ranking al terminar la partida */
        if (terminada && juego->ranking) {
            ranking_renderizar(juego->renderer, juego->fuenteGrande,
                               juego->fuenteChica, juego->ranking,
                               (int)juego->anchoVentana,
                               (int)juego->altoVentana);

            /* Mensaje adicional para reiniciar */
            SDL_Texture *tMsg = texto_crear_textura(juego->renderer, juego->fuenteChica,
                                                    "Presione ENTER para jugar otra vez",
                                                    (SDL_Color){100,255,100,255});
            if (tMsg) {
                int w, h;
                SDL_QueryTexture(tMsg, NULL, NULL, &w, &h);
                SDL_Rect dst = { (juego->anchoVentana - w) / 2,
                                 juego->altoVentana - 50, w, h };
                SDL_RenderCopy(juego->renderer, tMsg, NULL, &dst);
                SDL_DestroyTexture(tMsg);
            }
        }

    /* ---- Botón Cancelar ---- */
        if (!terminada) {
            SDL_Rect botonCancelar = { juego->anchoVentana - 170, 20, 150, 40 };
            SDL_SetRenderDrawColor(juego->renderer, 200, 50, 50, 255);
            SDL_RenderFillRect(juego->renderer, &botonCancelar);
            SDL_SetRenderDrawColor(juego->renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(juego->renderer, &botonCancelar);
            
            SDL_Texture *tCancelar = texto_crear_textura(juego->renderer, juego->fuenteChica, "Cancelar", blanco);
            if (tCancelar) {
                int w, h;
                SDL_QueryTexture(tCancelar, NULL, NULL, &w, &h);
                SDL_Rect dst = { botonCancelar.x + (botonCancelar.w - w)/2, 
                                botonCancelar.y + (botonCancelar.h - h)/2, w, h };
                SDL_RenderCopy(juego->renderer, tCancelar, NULL, &dst);
                SDL_DestroyTexture(tCancelar);
            }
        }
    }

    /* ---- Composición final ---- */
    graficos_cambiar_framebuffer(juego->renderer, NULL);
    graficos_renderizar(juego->renderer, juego->framebuffers, FB_CANT);
}

void juego_destruir(tJuego *juego)
{
    if (juego->ranking) {
        vector_destroy(juego->ranking);
        juego->ranking = NULL;
    }

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