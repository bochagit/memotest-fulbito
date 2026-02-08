/**
 * @file texto.c
 * @brief Implementacion del modulo 'texto'.
 */

#include "texto.h"
#include <stdio.h>


tError texto_inicializar(void)
{
    if (TTF_Init() != 0) {
        fprintf(stderr, "No se pudo inicializar SDL_ttf: %s\n", TTF_GetError());
        return ERR_SDL_TTF;
    }

    return TODO_OK;
}

TTF_Font* texto_cargar_fuente(const char *path, uint32_t tam)
{
    TTF_Font *fuente = TTF_OpenFont(path, tam);
    if (!fuente) {
        fprintf(stderr, "No se pudo cargar la fuente \"%s:\" %s\n", path, TTF_GetError());
    }

    return fuente;
}

void texto_destruir_fuente(TTF_Font *fuente)
{
    TTF_CloseFont(fuente);
}

SDL_Texture* texto_crear_textura(SDL_Renderer *renderer, TTF_Font *fuente, const char* texto, SDL_Color color)
{
    SDL_Surface * superficie = TTF_RenderUTF8_Blended(fuente, texto, color); // Permite utilizar caracteres UTF8
    if (!superficie) {
        fprintf(stderr, "%s\n", TTF_GetError());
        return NULL;
    }

    SDL_Texture *textura = SDL_CreateTextureFromSurface(renderer, superficie);

    SDL_FreeSurface(superficie); // Una vez creada la textura, la superficie ya no es necesaria

    if (!textura) {
        fprintf(stderr, "%s\n", SDL_GetError());
        return NULL;
    }

    SDL_SetTextureBlendMode(textura, SDL_BLENDMODE_BLEND);

    return textura;
}

void texto_finalizar(void)
{
    TTF_Quit();
}
