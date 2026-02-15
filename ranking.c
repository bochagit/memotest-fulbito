
#include "ranking.h"
#include "texto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
   FUNCIONES INTERNAS
  */

/* Intercambia dos bloques de memoria usando un buffer auxiliar.
 */
static void intercambiar(void *a, void *b, size_t tam, void *buffer)
{
    memcpy(buffer, a, tam);
    memcpy(a, b, tam);
    memcpy(b, buffer, tam);
}

/* Comparación descendente por puntaje (mayor puntaje primero).
 */
static int cmpPuntajeDesc(const void *a, const void *b)
{
    const tRankingEntry *ra = (const tRankingEntry*)a;
    const tRankingEntry *rb = (const tRankingEntry*)b;
    return rb->puntaje - ra->puntaje;
}

/*
   FUNCIONES PÚBLICAS
    */

void ranking_ordenar_seleccion(tVector *vector, Cmp cmp)
{
    if (!vector || vector_size(vector) < 2)
        return;

    void *bufferSwap = malloc(vector->elemSize);
    if (!bufferSwap)
        return;

    size_t n = vector_size(vector);
    char *ult = (char*)vector->data + (n - 1) * vector->elemSize;
    char *m;

    for (char *i = (char*)vector->data; i < ult; i += vector->elemSize)
    {
        m = i;
        for (char *j = i + vector->elemSize; j <= ult; j += vector->elemSize)
        {
            if (cmp(j, m) < 0)
            {
                m = j;
            }
        }

        intercambiar(m, i, vector->elemSize, bufferSwap);
    }

    free(bufferSwap);
}


tVector* ranking_cargar(const char *ruta)
{
    tVector *ranking = vector_create(sizeof(tRankingEntry));
    if (!ranking)
        return NULL;

    FILE *archivo = fopen(ruta, "r");
    if (!archivo)
        return ranking;   /* archivo no existe aún: vector vacío */

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo))
    {
        tRankingEntry entry;
        memset(&entry, 0, sizeof(entry));

        /* Formato: nombre;puntaje */
        char *sep = strchr(linea, ';');
        if (!sep) continue;

        size_t lenNombre = (size_t)(sep - linea);
        if (lenNombre >= MAX_NOMBRE_RANKING)
            lenNombre = MAX_NOMBRE_RANKING - 1;

        strncpy(entry.nombre, linea, lenNombre);
        entry.nombre[lenNombre] = '\0';
        entry.puntaje = atoi(sep + 1);

        vector_push_back(ranking, &entry);
    }

    fclose(archivo);
    return ranking;
}


int ranking_guardar(const char *ruta, tVector *ranking)
{
    if (!ranking) return -1;

    FILE *archivo = fopen(ruta, "w");
    if (!archivo) return -1;

    size_t n = vector_size(ranking);
    for (size_t i = 0; i < n; ++i)
    {
        tRankingEntry *e = (tRankingEntry*)vector_get(ranking, i);
        if (e)
            fprintf(archivo, "%s;%d\n", e->nombre, e->puntaje);
    }

    fclose(archivo);
    return 0;
}


void ranking_agregar(tVector *ranking, const char *nombre, int puntaje)
{
    if (!ranking || !nombre) return;

    tRankingEntry entry;
    memset(&entry, 0, sizeof(entry));
    strncpy(entry.nombre, nombre, MAX_NOMBRE_RANKING - 1);
    entry.nombre[MAX_NOMBRE_RANKING - 1] = '\0';
    entry.puntaje = puntaje;

    vector_push_back(ranking, &entry);

    /* Ordenar de mayor a menor puntaje */
    ranking_ordenar_seleccion(ranking, cmpPuntajeDesc);

    /* Recortar al top MAX_RANKING */
    while (vector_size(ranking) > MAX_RANKING)
        vector_remove_at(ranking, vector_size(ranking) - 1);
}


void ranking_renderizar(SDL_Renderer *renderer, TTF_Font *fuenteGrande,
                        TTF_Font *fuenteChica, tVector *ranking,
                        int anchoVentana, int altoVentana)
{
    if (!renderer || !ranking) return;

    size_t n = vector_size(ranking);
    if (n > MAX_RANKING) n = MAX_RANKING;

    /* Dimensiones del panel */
    int anchoPanel   = 500;
    int altoLinea     = 28;
    int altoTitulo    = 55;
    int padding       = 20;
    int altoPanel     = altoTitulo + padding + (int)n * altoLinea + padding;
    int panelX        = (anchoVentana - anchoPanel) / 2;
    int panelY        = (altoVentana - altoPanel) / 2;

    /* Fondo semi-transparente para todo el fondo */
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect bgFull = { 0, 0, anchoVentana, altoVentana };
    SDL_RenderFillRect(renderer, &bgFull);

    /* Panel del ranking */
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 230);
    SDL_Rect panelRect = { panelX, panelY, anchoPanel, altoPanel };
    SDL_RenderFillRect(renderer, &panelRect);

    /* Borde del panel */
    SDL_SetRenderDrawColor(renderer, 255, 200, 50, 255);
    SDL_RenderDrawRect(renderer, &panelRect);

    /* Título "RANKING TOP 10" */
    if (fuenteGrande)
    {
        SDL_Color dorado = {255, 200, 50, 255};
        SDL_Texture *tTitulo = texto_crear_textura(renderer, fuenteGrande,
                                                   "RANKING TOP 10", dorado);
        if (tTitulo)
        {
            int w, h;
            SDL_QueryTexture(tTitulo, NULL, NULL, &w, &h);
            SDL_Rect dst = { panelX + (anchoPanel - w) / 2, panelY + 5, w, h };
            SDL_RenderCopy(renderer, tTitulo, NULL, &dst);
            SDL_DestroyTexture(tTitulo);
        }
    }

    /* Entradas del ranking */
    if (fuenteChica)
    {
        int yBase = panelY + altoTitulo + padding;
        SDL_Color blanco  = {255, 255, 255, 255};
        SDL_Color dorado  = {255, 215, 0, 255};
        SDL_Color plata   = {192, 192, 192, 255};
        SDL_Color bronce  = {205, 127, 50, 255};

        for (size_t i = 0; i < n; ++i)
        {
            tRankingEntry *e = (tRankingEntry*)vector_get(ranking, i);
            if (!e) continue;

            char linea[128];
            snprintf(linea, sizeof(linea), "%2d. %-20s %d pts",
                     (int)(i + 1), e->nombre, e->puntaje);

            SDL_Color color;
            if (i == 0)      color = dorado;
            else if (i == 1) color = plata;
            else if (i == 2) color = bronce;
            else             color = blanco;

            SDL_Texture *tLinea = texto_crear_textura(renderer, fuenteChica,
                                                      linea, color);
            if (tLinea)
            {
                int w, h;
                SDL_QueryTexture(tLinea, NULL, NULL, &w, &h);
                SDL_Rect dst = { panelX + padding, yBase + (int)i * altoLinea, w, h };
                SDL_RenderCopy(renderer, tLinea, NULL, &dst);
                SDL_DestroyTexture(tLinea);
            }
        }

        /* Mensaje para salir */
        SDL_Texture *tMsg = texto_crear_textura(renderer, fuenteChica,
                                                "Presione ESC para salir",
                                                (SDL_Color){180,180,180,255});
        if (tMsg)
        {
            int w, h;
            SDL_QueryTexture(tMsg, NULL, NULL, &w, &h);
            SDL_Rect dst = { panelX + (anchoPanel - w) / 2,
                             panelY + altoPanel + 10, w, h };
            SDL_RenderCopy(renderer, tMsg, NULL, &dst);
            SDL_DestroyTexture(tMsg);
        }
    }
}
