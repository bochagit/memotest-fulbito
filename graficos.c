/**
 * @file graficos.c
 * @brief Implementacion del modulo 'graficos'.
 */

#include "graficos.h"
#include "stdio.h"
#include <stdlib.h>

/**
 * @brief Paleta de colores.
 */
SDL_Color colores[] =
{
    {0,   0,   0,   255}, // N[0] - Negro
    {255, 255, 0,   255}, // Y[1] - Amarillo
    {255, 255, 255, 255}, // B[2] - Blanco
    {0,   150, 60,  255}, // V[3] - Verde
    {255,   0,  0,  255}, // R[4] - Rojo
    {0,   0,    0,    0}  // T[5] - Transparente
};


void graficos_dibujar(SDL_Renderer *renderer, const uint8_t dibujo[][PIXELES_X_LADO], int32_t oX, int32_t oY)
{
    int32_t offsetX = oX * (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING);
    int32_t offsetY = oY * (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING);
    uint8_t transparencia = rand() & 255;

    for (int32_t y = 0; y < PIXELES_X_LADO; y++) {
        for (int32_t x = 0; x < PIXELES_X_LADO; x++) {
            SDL_SetRenderDrawColor(renderer,
                                   colores[dibujo[y][x]].r,
                                   colores[dibujo[y][x]].g,
                                   colores[dibujo[y][x]].b,
                                   colores[dibujo[y][x]].a == 0 ? 0 : transparencia);
            SDL_Rect pixel = {offsetX + TAM_PIXEL*x, offsetY + TAM_PIXEL*y, TAM_PIXEL, TAM_PIXEL};
            SDL_RenderFillRect(renderer, &pixel);
        }
    }
}

void graficos_dibujar_textura(SDL_Renderer *renderer, SDL_Texture *textura, SDL_Rect *origen, int32_t posX, int32_t posY, float escalaHor, float escalaVer, double angulo, uint8_t flipHor, uint8_t flipVer)
{
    SDL_Rect destino = {
        posX,
        posY,
        0,
        0
    };

    SDL_QueryTexture(textura, NULL, NULL, &destino.w, &destino.h);
    destino.w = (int32_t)(destino.w * escalaHor);
    destino.h = (int32_t)(destino.h * escalaVer);

    SDL_Point centro = {
        (int32_t)(destino.w / 2),
        (int32_t)(destino.h / 2),
    };

    SDL_RendererFlip flip = (flipHor ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE) |
                            (flipVer ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);

    SDL_RenderCopyEx(renderer, textura, origen, &destino, angulo, &centro, flip);
}

void graficos_borrar_pantalla(SDL_Renderer *renderer, const SDL_Color *color)
{
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    SDL_RenderClear(renderer);
}

SDL_Texture* graficos_crear_framebuffer(SDL_Renderer *renderer, uint32_t anchoVentana, uint32_t altoVentana)
{
    SDL_Texture *framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, anchoVentana, altoVentana);
    if (!framebuffer) {
        fprintf(stderr, "Error: No se pudo crear el framebuffer: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_SetTextureBlendMode(framebuffer, SDL_BLENDMODE_BLEND);

    return framebuffer;
}

void graficos_cambiar_framebuffer(SDL_Renderer *renderer, SDL_Texture *target)
{
    SDL_SetRenderTarget(renderer, target);
}

void graficos_renderizar(SDL_Renderer *renderer, SDL_Texture* const *framebuffers, uint32_t cantFramebuffers)
{
    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (uint32_t i = 0; i < cantFramebuffers; ++i) {
        if (framebuffers[i]) {
            SDL_RenderCopy(renderer, framebuffers[i], NULL, NULL);
        }
    }

    SDL_RenderPresent(renderer);
}
