/**
 * @file memoria.c
 * @brief Implementación del juego de la memoria.
 *
 * Usa el TDA Vector para almacenar cartas y texturas.
 * Soporta 2 sets de figuras, 1 o 2 jugadores, hover y sonidos.
 */

#include "memoria.h"
#include "imagenes.h"
#include "vector.h"
#include "sonidos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ---- Constantes ---- */
#define TIEMPO_MOSTRAR_MS  700
#define PUNTOS_MIN         10
#define PUNTOS_MAX         50
#define TAM_CARTA_GEN     128   /* px para texturas generadas */
#define MARGEN_SUPERIOR    80   /* px reservados arriba para HUD */
#define BORDE_CARTA        4    /* grosor del borde de la carta */
#define OPACIDAD_CARTA    100  /* opacidad del fondo de carta (0-255) */

/* Rutas de imágenes: Set 1 – clubes argentinos */
static const char *RUTAS_SET1[] = {
    "img/pareja1_boca.png",
    "img/pareja2_river.png",
    "img/pareja3_sanlorenzo.png",
    "img/pareja4_huracan.png",
    "img/pareja5_independiente.png",
    "img/pareja6_racing.png",
    "img/pareja7_velez.png",
    "img/pareja8_belgrano.png",
    "img/pareja9_newells.png",
    "img/pareja10_argentinos.png",
};
#define TOTAL_SET1 10

/* Rutas de imágenes: Set 2 – Champions League */
static const char *RUTAS_SET2[] = {
    "img/2_pareja1barcelona.png",
    "img/2_pareja2realmadrid.png",
    "img/2_pareja3psg.png",
    "img/2_pareja4chelsea.png",
    "img/2_pareja5bayernmunchen.png",
    "img/2_pareja6arsenal.png",
    "img/2_pareja7inter.png",
    "img/2_pareja8juventus.png",
    "img/2_pareja9liverpool.png",
    "img/2_pareja10manchestercity.png",
};
#define TOTAL_SET2 10

/* ---- Tipos internos ---- */
typedef struct {
    int  idPareja;
    int  indiceTextura;   /* índice dentro del vector de texturas únicas */
    int  puntos;
    uint8_t descubierta;
    uint8_t encontrada;
} tCarta;

typedef struct {
    int puntos;
    int aciertos;
    int intentos;
    int racha;
} tEstadisticasJug;

struct sMemoria {
    tVector *cartas;       /* vector de tCarta */
    tVector *texturas;     /* vector de SDL_Texture* (una por par) */
    SDL_Texture *texturaReverso;
    int filas;
    int columnas;
    int setFiguras;
    SDL_Renderer *renderer;
    int seleccionado1;
    int seleccionado2;
    uint32_t tiempoEspera;
    int cantJugadores;
    int turnoActual;       /* 0 o 1 */
    tEstadisticasJug estadisticas[2];
    int usarSonidos;
    tSonido *sonidoAcierto;
    tSonido *sonidoFallo;
    tSonido *sonidoPrimera;
    int cartaHover;        /* -1 ninguna */
};

/* ---- Helpers ---- */
static int _rand_entre(int a, int b)
{
    return a + rand() % (b - a + 1);
}

static SDL_Texture* _crear_textura_color(SDL_Renderer *renderer, SDL_Color color,
                                         int ancho, int alto)
{
    SDL_Surface *sup = SDL_CreateRGBSurface(0, ancho, alto, 32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#else
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#endif
    );
    if (!sup) return NULL;

    SDL_FillRect(sup, NULL, SDL_MapRGBA(sup->format, color.r, color.g, color.b, color.a));

    /* Borde blanco decorativo */
    uint32_t blanco = SDL_MapRGBA(sup->format, 255, 255, 255, 255);
    SDL_Rect borde;
    borde = (SDL_Rect){0, 0, ancho, 4}; SDL_FillRect(sup, &borde, blanco);
    borde = (SDL_Rect){0, alto-4, ancho, 4}; SDL_FillRect(sup, &borde, blanco);
    borde = (SDL_Rect){0, 0, 4, alto}; SDL_FillRect(sup, &borde, blanco);
    borde = (SDL_Rect){ancho-4, 0, 4, alto}; SDL_FillRect(sup, &borde, blanco);

    /* Diamante central decorativo */
    int cx = ancho/2, cy = alto/2, r = ancho/6;
    for (int dy = -r; dy <= r; ++dy) {
        int amplitud = r - abs(dy);
        SDL_Rect linea = { cx - amplitud, cy + dy, amplitud*2, 1 };
        SDL_FillRect(sup, &linea, blanco);
    }

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, sup);
    SDL_FreeSurface(sup);
    return tex;
}

static void _calcular_rect_carta(tMemoria *m, int indice, SDL_Rect *dst,
                                 int anchoV, int altoV)
{
    int col  = indice % m->columnas;
    int fila = indice / m->columnas;
    int pad  = 8;
    int areaW = anchoV - pad * 2;
    int areaH = altoV - MARGEN_SUPERIOR - pad * 2;
    int w = areaW / m->columnas - pad;
    int h = areaH / m->filas    - pad;
    dst->x = pad + col  * (w + pad);
    dst->y = MARGEN_SUPERIOR + pad + fila * (h + pad);
    dst->w = w;
    dst->h = h;
}

/* Dibuja el borde decorativo de una carta */
static void _dibujar_borde_carta(SDL_Renderer *renderer, const SDL_Rect *rect, int encontrada)
{
    /* Borde exterior (más grueso) */
    if (encontrada) {
        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);  /* Verde para encontradas */
    } else {
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255); /* Gris oscuro */
    }

    for (int i = 0; i < BORDE_CARTA; ++i) {
        SDL_Rect borde = { rect->x + i, rect->y + i,
                          rect->w - i*2, rect->h - i*2 };
        SDL_RenderDrawRect(renderer, &borde);
    }

    /* Borde interior decorativo */
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 200);
    SDL_Rect bordeInt = {
        rect->x + BORDE_CARTA + 2,
        rect->y + BORDE_CARTA + 2,
        rect->w - (BORDE_CARTA + 2) * 2,
        rect->h - (BORDE_CARTA + 2) * 2
    };
    SDL_RenderDrawRect(renderer, &bordeInt);
}

/* ---- Funciones públicas ---- */

tMemoria* memoria_crear(SDL_Renderer *renderer, int filas, int columnas,
                        int setFiguras, int usarSonidos, int cantJugadores)
{
    if (!renderer || filas <= 0 || columnas <= 0) return NULL;
    int total = filas * columnas;
    if (total % 2 != 0) return NULL;
    int pares = total / 2;

    tMemoria *m = malloc(sizeof(tMemoria));
    if (!m) return NULL;
    memset(m, 0, sizeof(tMemoria));
    m->filas        = filas;
    m->columnas     = columnas;
    m->setFiguras   = setFiguras;
    m->renderer     = renderer;
    m->cantJugadores = (cantJugadores >= 2) ? 2 : 1;
    m->turnoActual  = 0;
    m->cartas   = vector_create(sizeof(tCarta));
    m->texturas = vector_create(sizeof(SDL_Texture*));
    if (!m->cartas || !m->texturas) { free(m); return NULL; }

    const char *rutaDorso = NULL;
    if (setFiguras == 1){
        rutaDorso = "img/dorso_lpf.png";
    } else if (setFiguras == 2){
        rutaDorso = "img/dorso_champions.png";
    }

    if (rutaDorso) m->texturaReverso = imagenes_cargar_gpu(renderer, rutaDorso);

    srand((unsigned)time(NULL));

    /* Cargar una textura por cada par */
    for (int id = 0; id < pares; ++id) {
        SDL_Texture *tex = NULL;
        if (setFiguras == 1 && id < TOTAL_SET1) {
            tex = imagenes_cargar_gpu(renderer, RUTAS_SET1[id]);
        }
        if (setFiguras == 2 && id < TOTAL_SET2) {
            tex = imagenes_cargar_gpu(renderer, RUTAS_SET2[id]);
        }
        if (!tex) {
            /* Fallback: generar textura de color */
            SDL_Color fb = { (uint8_t)(50+id*20), (uint8_t)(100+id*15),
                             (uint8_t)(200-id*15), 255 };
            tex = _crear_textura_color(renderer, fb, TAM_CARTA_GEN, TAM_CARTA_GEN);
        }
        if (!tex) {
            for (size_t k = 0; k < vector_size(m->texturas); ++k) {
                SDL_Texture **pt = (SDL_Texture**)vector_get(m->texturas, k);
                if (pt && *pt) SDL_DestroyTexture(*pt);
            }
            vector_destroy(m->texturas);
            vector_destroy(m->cartas);
            if (m->texturaReverso) SDL_DestroyTexture(m->texturaReverso);
            free(m);
            return NULL;
        }
        vector_push_back(m->texturas, &tex);

        int puntosPareja = _rand_entre(PUNTOS_MIN, PUNTOS_MAX);
        for (int rep = 0; rep < 2; ++rep) {
            tCarta carta = {0};
            carta.idPareja       = id;
            carta.indiceTextura  = id;
            carta.puntos         = puntosPareja;
            carta.descubierta    = 0;
            carta.encontrada     = 0;
            vector_push_back(m->cartas, &carta);
        }
    }

    /* Fisher-Yates */
    size_t n = vector_size(m->cartas);
    for (size_t i = n - 1; i > 0; --i) {
        size_t j = rand() % (i + 1);
        if (i != j) {
            tCarta tmp;
            tCarta *ci = (tCarta*)vector_get(m->cartas, i);
            tCarta *cj = (tCarta*)vector_get(m->cartas, j);
            tmp = *ci; *ci = *cj; *cj = tmp;
        }
    }

    m->seleccionado1 = -1;
    m->seleccionado2 = -1;
    m->tiempoEspera  = 0;
    m->cartaHover    = -1;
    memset(m->estadisticas, 0, sizeof(m->estadisticas));

    m->usarSonidos   = usarSonidos;
    m->sonidoAcierto = NULL;
    m->sonidoFallo   = NULL;
    m->sonidoPrimera = NULL;
    if (usarSonidos) {
        m->sonidoAcierto = sonidos_cargar("snd/Acierto_parejas.mp3");
        m->sonidoFallo   = sonidos_cargar("snd/No_acierto.mp3");
        m->sonidoPrimera = sonidos_cargar("snd/Seleccion_primera.mp3");
    }
    return m;
}

void memoria_destruir(tMemoria *m)
{
    if (!m) return;
    /* Destruir texturas únicas (una por par) */
    for (size_t i = 0; i < vector_size(m->texturas); ++i) {
        SDL_Texture **pt = (SDL_Texture**)vector_get(m->texturas, i);
        if (pt && *pt) SDL_DestroyTexture(*pt);
    }
    vector_destroy(m->texturas);
    vector_destroy(m->cartas);
    if (m->texturaReverso) SDL_DestroyTexture(m->texturaReverso);
    if (m->sonidoAcierto) sonidos_destruir(m->sonidoAcierto);
    if (m->sonidoFallo)   sonidos_destruir(m->sonidoFallo);
    if (m->sonidoPrimera) sonidos_destruir(m->sonidoPrimera);
    free(m);
}

tError memoria_procesar_evento(tMemoria *m, const SDL_Event *ev)
{
    if (!m || !ev) return ERR_MEMORIA;

    int anchoV, altoV;
    SDL_GetRendererOutputSize(m->renderer, &anchoV, &altoV);

    /* ---- Hover ---- */
    if (ev->type == SDL_MOUSEMOTION) {
        int mx = ev->motion.x, my = ev->motion.y;
        m->cartaHover = -1;
        for (size_t i = 0; i < vector_size(m->cartas); ++i) {
            SDL_Rect dst;
            _calcular_rect_carta(m, (int)i, &dst, anchoV, altoV);
            if (mx >= dst.x && mx <= dst.x+dst.w && my >= dst.y && my <= dst.y+dst.h) {
                tCarta *c = (tCarta*)vector_get(m->cartas, i);
                if (c && !c->encontrada) m->cartaHover = (int)i;
                break;
            }
        }
    }

    /* ---- Clic ---- */
    if (ev->type == SDL_MOUSEBUTTONDOWN && ev->button.button == SDL_BUTTON_LEFT) {
        int mx = ev->button.x, my = ev->button.y;
        for (size_t i = 0; i < vector_size(m->cartas); ++i) {
            SDL_Rect dst;
            _calcular_rect_carta(m, (int)i, &dst, anchoV, altoV);
            if (mx >= dst.x && mx <= dst.x+dst.w && my >= dst.y && my <= dst.y+dst.h) {
                tCarta *c = (tCarta*)vector_get(m->cartas, i);
                if (!c || c->encontrada || c->descubierta) break;
                if (m->seleccionado2 != -1 && m->tiempoEspera > 0) break;

                if (m->seleccionado1 == -1) {
                    c->descubierta = 1;
                    m->seleccionado1 = (int)i;
                    if (m->usarSonidos && m->sonidoPrimera) sonidos_reproducir(m->sonidoPrimera, 1);
                } else if (m->seleccionado1 != (int)i) {
                    c->descubierta = 1;
                    m->seleccionado2 = (int)i;
                    m->tiempoEspera  = TIEMPO_MOSTRAR_MS;
                }
                break;
            }
        }
    }
    return TODO_OK;
}

void memoria_actualizar(tMemoria *m, uint32_t deltaMs)
{
    if (!m) return;
    if (m->seleccionado2 == -1 || m->tiempoEspera == 0) return;

    if (deltaMs >= m->tiempoEspera) {
        tCarta *c1 = (tCarta*)vector_get(m->cartas, m->seleccionado1);
        tCarta *c2 = (tCarta*)vector_get(m->cartas, m->seleccionado2);
        int jug = m->turnoActual;
        m->estadisticas[jug].intentos++;

        if (c1 && c2 && c1->idPareja == c2->idPareja) {
            c1->encontrada = 1;
            c2->encontrada = 1;
            m->estadisticas[jug].aciertos++;
            m->estadisticas[jug].racha++;
            float mult = 1.0f + 0.25f * (m->estadisticas[jug].racha - 1);
            m->estadisticas[jug].puntos += (int)(c1->puntos * mult + 0.5f);
            if (m->usarSonidos && m->sonidoAcierto)
                sonidos_reproducir(m->sonidoAcierto, 1);
            /* Tras acierto el mismo jugador sigue */
        } else {
            if (c1) c1->descubierta = 0;
            if (c2) c2->descubierta = 0;
            m->estadisticas[jug].racha = 0;
            if (m->usarSonidos && m->sonidoFallo)
                sonidos_reproducir(m->sonidoFallo, 1);
            if (m->cantJugadores == 2)
                m->turnoActual = 1 - m->turnoActual;
        }
        m->seleccionado1 = -1;
        m->seleccionado2 = -1;
        m->tiempoEspera  = 0;
    } else {
        m->tiempoEspera -= deltaMs;
    }
}

void memoria_renderizar(tMemoria *m, SDL_Renderer *renderer)
{
    if (!m || !renderer) return;
    int anchoV, altoV;
    SDL_GetRendererOutputSize(renderer, &anchoV, &altoV);

    for (size_t i = 0; i < vector_size(m->cartas); ++i) {
        tCarta *c = (tCarta*)vector_get(m->cartas, i);
        SDL_Rect dst;
        _calcular_rect_carta(m, (int)i, &dst, anchoV, altoV);

        if (c->descubierta || c->encontrada) {
            /* Fondo semi-transparente blanco/gris claro */
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 250, 250, 250, OPACIDAD_CARTA);
            SDL_RenderFillRect(renderer, &dst);

            /* Calcular área para el logo (60% del tamaño de la carta, centrado) */
            int logoSize = (dst.w < dst.h ? dst.w : dst.h) * 60 / 100;
            SDL_Rect logoRect = {
                dst.x + (dst.w - logoSize) / 2,
                dst.y + (dst.h - logoSize) / 2,
                logoSize,
                logoSize
            };

            /* Mostrar la textura del logo centrado y más pequeño */
            SDL_Texture **pt = (SDL_Texture**)vector_get(m->texturas, c->indiceTextura);
            if (pt && *pt) SDL_RenderCopy(renderer, *pt, NULL, &logoRect);

            /* Borde de la carta */
            _dibujar_borde_carta(renderer, &dst, c->encontrada);

            if (c->encontrada) {
                /* Overlay verde semi-transparente */
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 0, 200, 0, 40);
                SDL_RenderFillRect(renderer, &dst);
            }
        } else {
            /* Fondo semi-transparente para carta boca abajo */
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, OPACIDAD_CARTA);
            SDL_RenderFillRect(renderer, &dst);

            if (m->texturaReverso){
                /* Calcular área para el dorso (70% del tamaño, centrado) */
                int dorsoSize = (dst.w < dst.h ? dst.w : dst.h) * 70 / 100;
                SDL_Rect dorsoRect = {
                    dst.x + (dst.w - dorsoSize) / 2,
                    dst.y + (dst.h - dorsoSize) / 2,
                    dorsoSize,
                    dorsoSize
                };
                SDL_RenderCopy(renderer, m->texturaReverso, NULL, &dorsoRect);
            } else {
                /* Fallback: patrón decorativo */
                SDL_SetRenderDrawColor(renderer, 100, 120, 140, 100);
                SDL_Rect interior = { dst.x + 15, dst.y + 15, dst.w - 30, dst.h - 30 };
                SDL_RenderFillRect(renderer, &interior);
            }

            /* Borde de la carta */
            _dibujar_borde_carta(renderer, &dst, 0);
        }

        /* Efecto hover */
        if ((int)i == m->cartaHover && !c->encontrada) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 60);
            SDL_RenderFillRect(renderer, &dst);
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            for (int b = 0; b < 3; ++b) {
                SDL_Rect highlight = { dst.x + b, dst.y + b, dst.w - b*2, dst.h - b*2 };
                SDL_RenderDrawRect(renderer, &highlight);
            }
        }
    }
}

/* ---- Estadísticas ---- */

void memoria_obtener_estadisticas(tMemoria *m, int *puntos, int *aciertos,
                                  int *intentos, int *racha)
{
    if (!m) return;
    int p = 0, a = 0, it = 0, r = 0;
    for (int j = 0; j < m->cantJugadores; ++j) {
        p  += m->estadisticas[j].puntos;
        a  += m->estadisticas[j].aciertos;
        it += m->estadisticas[j].intentos;
    }
    r = m->estadisticas[m->turnoActual].racha;
    if (puntos)   *puntos   = p;
    if (aciertos) *aciertos = a;
    if (intentos) *intentos = it;
    if (racha)    *racha    = r;
}

void memoria_obtener_estadisticas_jugador(tMemoria *m, int jugador,
                                          int *puntos, int *aciertos,
                                          int *intentos, int *racha)
{
    if (!m || jugador < 0 || jugador >= 2) return;
    if (puntos)   *puntos   = m->estadisticas[jugador].puntos;
    if (aciertos) *aciertos = m->estadisticas[jugador].aciertos;
    if (intentos) *intentos = m->estadisticas[jugador].intentos;
    if (racha)    *racha    = m->estadisticas[jugador].racha;
}

int memoria_obtener_turno(tMemoria *m)
{
    return m ? m->turnoActual : 0;
}

int memoria_partida_terminada(tMemoria *m)
{
    if (!m) return 1;
    for (size_t i = 0; i < vector_size(m->cartas); ++i) {
        tCarta *c = (tCarta*)vector_get(m->cartas, i);
        if (c && !c->encontrada) return 0;
    }
    return 1;
}
