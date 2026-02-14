/**
 * @file presentacion.c
 * @brief Implementación de la pantalla de presentación con entrada de nombre.
 */

#include "presentacion.h"
#include "imagenes.h"
#include "texto.h"
#include "sonidos.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <stdio.h>

// Tiempo de parpadeo del cursor en ms
#define CARET_BLINK_MS 500

tError presentacion_mostrar(SDL_Renderer *renderer, TTF_Font *fuente, const char *fondoPath, const char *sonidoPath, char *outName, size_t maxLen)
{
    if (!renderer || !fuente || !outName || maxLen == 0) return ERR_MEMORIA;

    /* Cargar fuente personalizada para la presentación */
    TTF_Font *fuentePresentacion = texto_cargar_fuente("fnt/font4.TTF", 36);
    TTF_Font *fuenteUsada = fuentePresentacion ? fuentePresentacion : fuente;

    SDL_Texture *fondo = imagenes_cargar_gpu(renderer, fondoPath);
    tSonido *sonido = NULL;
    if (sonidoPath) {
        sonido = sonidos_cargar(sonidoPath);
        if (sonido) {
            sonidos_reproducir(sonido, -1);
        }
    }

    /* Obtener dimensiones de la ventana */
    int anchoVentana, altoVentana;
    SDL_GetRendererOutputSize(renderer, &anchoVentana, &altoVentana);

    /* Dimensiones del recuadro centrado */
<<<<<<< HEAD
    int recuadroAncho = 700;
=======
    int recuadroAncho = 800;
>>>>>>> Yanil
    int recuadroAlto  = 180;
    if (recuadroAncho > anchoVentana - 40) recuadroAncho = anchoVentana - 40;
    int recuadroX = (anchoVentana - recuadroAncho) / 2;
    int recuadroY = (altoVentana  - recuadroAlto)  / 2;

    char buffer[256] = {0};
    size_t len = 0;

    SDL_StartTextInput();
    int done = 0;
    uint32_t lastBlink = SDL_GetTicks();
    int caretVisible = 1;

    while (!done) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                SDL_StopTextInput();
                if (fuentePresentacion) texto_destruir_fuente(fuentePresentacion);
                if (sonido) sonidos_destruir(sonido);
                if (fondo) SDL_DestroyTexture(fondo);
                return ERR_SDL;
            } else if (ev.type == SDL_TEXTINPUT) {
                const char *txt = ev.text.text;
                size_t add = strlen(txt);
                if (len + add < sizeof(buffer) && len + add < maxLen) {
                    memcpy(buffer + len, txt, add);
                    len += add;
                    buffer[len] = '\0';
                }
            } else if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_BACKSPACE) {
                    if (len > 0) {
                        len--;
                        buffer[len] = '\0';
                    }
                } else if (ev.key.keysym.sym == SDLK_RETURN || ev.key.keysym.sym == SDLK_KP_ENTER) {
                    done = 1;
                    break;
                }
            }
        }

        uint32_t now = SDL_GetTicks();
        if (now - lastBlink >= CARET_BLINK_MS) {
            caretVisible = !caretVisible;
            lastBlink = now;
        }

        /* ---- Render ---- */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (fondo) {
            SDL_RenderCopy(renderer, fondo, NULL, NULL);
        }

        /* Recuadro semi-transparente centrado */
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_Rect recuadro = { recuadroX, recuadroY, recuadroAncho, recuadroAlto };
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_RenderFillRect(renderer, &recuadro);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 220);
        SDL_RenderDrawRect(renderer, &recuadro);
        /* Borde interior decorativo */
        SDL_Rect bordeInt = { recuadroX + 4, recuadroY + 4, recuadroAncho - 8, recuadroAlto - 8 };
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 120);
        SDL_RenderDrawRect(renderer, &bordeInt);

        /* Texto de instrucción centrado dentro del recuadro */
        SDL_Color blanco = {255, 255, 255, 255};
        SDL_Texture *tInstruccion = texto_crear_textura(renderer, fuenteUsada,
            "Ingrese su nombre:", blanco);
        if (tInstruccion) {
            int w, h;
            SDL_QueryTexture(tInstruccion, NULL, NULL, &w, &h);
            SDL_Rect dst = { recuadroX + (recuadroAncho - w) / 2,
                             recuadroY + 20, w, h };
            SDL_RenderCopy(renderer, tInstruccion, NULL, &dst);
            SDL_DestroyTexture(tInstruccion);
        }

        /* Campo de texto con fondo oscuro */
        int campoAncho = recuadroAncho - 60;
        int campoAlto  = 44;
        int campoX = recuadroX + 30;
        int campoY = recuadroY + 70;
        SDL_Rect campo = { campoX, campoY, campoAncho, campoAlto };
        SDL_SetRenderDrawColor(renderer, 30, 30, 50, 220);
        SDL_RenderFillRect(renderer, &campo);
        SDL_SetRenderDrawColor(renderer, 180, 180, 255, 200);
        SDL_RenderDrawRect(renderer, &campo);

        /* Texto del nombre dentro del campo */
        char display[300];
        if (caretVisible)
            snprintf(display, sizeof(display), "%s|", buffer);
        else
            snprintf(display, sizeof(display), "%s ", buffer);

        SDL_Texture *tNombre = texto_crear_textura(renderer, fuenteUsada, display, blanco);
        if (tNombre) {
            int w, h;
            SDL_QueryTexture(tNombre, NULL, NULL, &w, &h);
            /* Centrado vertical dentro del campo, alineado a la izquierda con padding */
            SDL_Rect dst = { campoX + 10, campoY + (campoAlto - h) / 2, w, h };
            SDL_RenderCopy(renderer, tNombre, NULL, &dst);
            SDL_DestroyTexture(tNombre);
        }

        /* Indicación de ENTER */
        SDL_Color gris = {180, 180, 180, 255};
        SDL_Texture *tEnter = texto_crear_textura(renderer, fuenteUsada,
            "(ENTER para continuar)", gris);
        if (tEnter) {
            int w, h;
            SDL_QueryTexture(tEnter, NULL, NULL, &w, &h);
            SDL_Rect dst = { recuadroX + (recuadroAncho - w) / 2,
                             recuadroY + recuadroAlto - h - 15, w, h };
            SDL_RenderCopy(renderer, tEnter, NULL, &dst);
            SDL_DestroyTexture(tEnter);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_StopTextInput();

    strncpy(outName, buffer, maxLen - 1);
    outName[maxLen - 1] = '\0';

    if (fuentePresentacion) texto_destruir_fuente(fuentePresentacion);

    if (sonido) {
        sonidos_destruir(sonido);
    }
    if (fondo) {
        SDL_DestroyTexture(fondo);
    }

    return TODO_OK;
}
