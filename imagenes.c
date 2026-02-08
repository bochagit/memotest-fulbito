/**
 * @file imagenes.c
 * @brief Implementacion del modulo 'imagenes'.
 */

#include "imagenes.h"
#include <stdio.h>

tFormatosImg imagenes_inicializar(void)
{
    tFormatosImg formatosImg = IMAGEN_BMP; // Soporte para BMP ya incluido
    int32_t flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int32_t init = IMG_Init(flags); // Inicializa SDL_image

    if ((init & flags) != flags) {
        fprintf(stderr,"Error: %s\n", IMG_GetError());
    }

    if (init & IMG_INIT_JPG) {
        formatosImg |= IMAGEN_JPG;
    }

    if (init & IMG_INIT_PNG) {
        formatosImg |= IMAGEN_PNG;
    }

    if (!init) {
        fprintf(stderr, "Error: No se pudo inicializar ningun formato de SDL_image %s\n", IMG_GetError());
    }

    return formatosImg;
}


SDL_Texture* imagenes_cargar_gpu(SDL_Renderer *renderer, const char *path)
{
    SDL_Texture *textura = IMG_LoadTexture(renderer, path);
    if (!textura) {
        fprintf(stderr, "Fallo la carga de la imagen \"%s:\" %s\n", path, IMG_GetError());
    }

    return textura;
}

SDL_Surface* imagenes_cargar_ram(const char *path)
{
    SDL_Surface *superficie = IMG_Load(path);
    if (!superficie) {
        fprintf(stderr, "Fallo la carga de la imagen en RAM \"%s:\" %s\n", path, IMG_GetError());
    }

    return superficie;
}

void imagenes_finalizar(void)
{
    IMG_Quit();
}



