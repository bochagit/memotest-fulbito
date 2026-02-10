/**
 * @file sonidos.c
 * @brief Implementacion del modulo 'sonidos'.
 */

#include "sonidos.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MIXER_CANAL_LIBRE -1 // Reproduce en el primer canal libre disponible
#define BUFFER_AUDIO 1024
#define MONO 1

struct sSonido{
    Mix_Chunk *chunk;
    uint8_t esTono;
};

tFormatosSnd sonidos_inicializar(void)
{
    tFormatosSnd formatosSnd = SONIDO_WAV; // Soporte para WAV ya incluido
    int32_t flags = MIX_INIT_MP3 | MIX_INIT_OGG;
    int32_t init = Mix_Init(flags); // Inicializa SDL_mixer

    if ((init & flags) != flags) {
        fprintf(stderr,"Error: %s\n", Mix_GetError());
    }

    if (init & MIX_INIT_MP3) {
        formatosSnd |= SONIDO_MP3;
    }

    if (init & MIX_INIT_OGG) {
        formatosSnd |= SONIDO_OGG;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MONO, BUFFER_AUDIO) < 0) { // Abre el dispositivo de audio en modo Mono
        fprintf(stderr, "Error: No se pudo abrir el dispositivo de audio: %s\n", Mix_GetError());
        Mix_Quit();
        return SONIDO_ERR;
    }

    Mix_Volume(-1, 20); // Reducion del volumen al 20% de forma fija a efectos de no molestar a quien inicie esta demo

    return formatosSnd;
}

tSonido* sonidos_cargar(const char *path)
{
    tSonido *sonido = malloc(sizeof(tSonido));
    if (!sonido) {
        return NULL;
    }

    sonido->esTono = 0;
    sonido->chunk = Mix_LoadWAV(path);
    if (!sonido->chunk) {
        fprintf(stderr, "Fallo la carga del sonido \"%s:\" %s\n", path, Mix_GetError());
        free(sonido);
        return NULL;
    }

    return sonido;
}

tSonido* sonidos_crear_tono(float frecuencia)
{
    tSonido *sonido = malloc(sizeof(tSonido));
    if (!sonido) {
        return NULL;
    }

    int16_t *buffer = malloc(sizeof(int16_t) * FREC_MUESTREO);
    if (!buffer) {
        free(sonido);
        fprintf(stderr, "No se pudo reservar memoria para el tono\n");
        return NULL;
    }

    for (int32_t i = 0; i < FREC_MUESTREO; i++) {
        buffer[i] = AMPLITUD_TONO * sin(2.0 * M_PI * frecuencia * i / FREC_MUESTREO); // Amplitud * seno(2Pi * frecuencia * dTiempo)
    }

    sonido->chunk = malloc(sizeof(Mix_Chunk));
    if (!sonido->chunk) {
        free(buffer);
        free(sonido);
        fprintf(stderr, "No se pudo reservar memoria para el tono\n");
        return NULL;
    }

    sonido->chunk->alen = FREC_MUESTREO * sizeof(int16_t); // Cantidad de bytes del buffer de audio
    sonido->chunk->abuf = (uint8_t*)buffer;
    sonido->chunk->volume = MIX_MAX_VOLUME;
    sonido->esTono = 1;

    return sonido;
}

void sonidos_reproducir(const tSonido *sonido, int32_t cantVeces)
{
    if (!sonido->chunk) {
        return;
    }

    int32_t loops;
    if (cantVeces == -1) {
        loops = -1; // -1 para reproducir de forma infinita
    } else {
        loops = (cantVeces > 0) ? (cantVeces - 1) : 0; // En mixer 0 para una vez, 1 para dos, etc.. y no acepta valores menores a -1
    }

    if (Mix_PlayChannel(MIXER_CANAL_LIBRE, sonido->chunk, loops) == -1) {
        fprintf(stderr, "No se pudo reproducir el sonido: %s\n", Mix_GetError());
    }
}

void sonidos_destruir(tSonido *sonido)
{
    if (!sonido) {
        return;
    }

    if (sonido->esTono) {
        if (sonido->chunk) {
            if (sonido->chunk->abuf) {
                free(sonido->chunk->abuf);
            }
            free(sonido->chunk);
        }
    } else {
        if (sonido->chunk) {
            Mix_FreeChunk(sonido->chunk);
        }
    }

    free(sonido);
}

void sonidos_finalizar(void)
{
    Mix_HaltChannel(-1);
    Mix_CloseAudio();
    Mix_Quit();
}
