

#ifndef SONIDOS_H_INCLUDED
#define SONIDOS_H_INCLUDED
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define AMPLITUD_TONO 8192
#define FREC_MUESTREO 44100

/**
 * @brief Formatos de audio soportados.
 */
typedef enum {
    SONIDO_ERR = -1,
    SONIDO_WAV = 0x00,
    SONIDO_MP3 = 0x01,
    SONIDO_OGG = 0x02,
} tFormatosSnd;

/**
 * @brief Estructura opaca para la gestion de recursos de audio.
 * * * Al ser opaca, oculta si el recurso es un Mix_Chunk cargado
 * desde el disco o un buffer de audio generado.
 */
typedef struct sSonido tSonido;

/**
 * @brief Inicializa los subsistemas de audio y abre el dispositivo de salida.
 *
 * @return tFormatosSnd Mascara de bits con los formatos cargados o SONIDO_ERR si el
 * dispositivo de audio no pudo abrirse.
 */
tFormatosSnd sonidos_inicializar(void);

/**
 * @brief Carga un archivo de audio desde el disco.
 *
 * @param path Ruta al archivo.
 *
 * @return tSonido* Puntero a la instancia o NULL si hubo error.
 */
tSonido* sonidos_cargar(const char *path);

/**
 * @brief Genera un tono puro de onda sinusoidal dada la frecuencia.
 *
 * @param frecuencia Valor en Hz del tono a generar.
 *
 * @return tSonido* Puntero a la instancia o NULL si hubo error.
 */
tSonido* sonidos_crear_tono(float frecuencia);

/**
 * @brief Reproduce un sonido una cantidad 'n' de veces.
 *
 * @param sonido Puntero constante a la instancia de sonido.
 * @param cantVeces Numero de repeticiones o -1 para bucle infinito.
 */
void sonidos_reproducir(const tSonido *sonido, int32_t cantVeces);

/**
 * @brief Libera la memoria asociada al sonido.
 */
void sonidos_destruir(tSonido *sonido);

/**
 * @brief Cierra el dispositivo de audio y finaliza SDL_mixer.
 */
void sonidos_finalizar(void);

#endif // SONIDOS_H_INCLUDED
