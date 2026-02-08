

#ifndef IMAGENES_H_INCLUDED
#define IMAGENES_H_INCLUDED
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

/**
 * @brief Enumeracion con los formatos de imagen soportados.
 */
typedef enum {
    IMAGEN_BMP  = 0x00,
    IMAGEN_JPG  = 0x01,
    IMAGEN_PNG  = 0x02,
} tFormatosImg;

/**
 * @brief Inicializa el subsistema SDL_image.
 * @return tFormatosImg Retorna una mascara de bits con los formatos cargados exitosamente.
 */
tFormatosImg imagenes_inicializar(void);

/**
 * @brief Carga una imagen directamente en la memoria de la GPU.
 * * * Se recomienda para imagenes que se renderizan frecuentemente y
 * permaneceran sin cambios durante su vida util.
 *
 * @param renderer Puntero al renderizador de SDL.
 * @param path Ruta al archivo.
 *
 * @return SDL_Texture* Puntero a la textura, o NULL si fallo la carga.
 *
 * @note La textura debe ser liberada con la funcion 'SDL_DestroyTexture' al finalizar su uso.
 */
SDL_Texture* imagenes_cargar_gpu(SDL_Renderer *renderer, const char *path);

/**
 * @brief Carga una imagen en la memoria RAM.
 * * * Se utiliza cuando se desea tener mayor control y acceso a la informacion de
 * la imagen, como por ejemplo para acceder a los valores de sus pixeles y/o modificarlos.
 *
 * @param path Ruta al archivo.
 * @return SDL_Surface* Puntero a la superficie en RAM, o NULL si fallo la carga.
 *
 * @note La superficie debe ser liberada con la funcion 'SDL_FreeSurface' al finalizar su uso.
 */
SDL_Surface* imagenes_cargar_ram(const char *path);

/**
 * @brief Libera los recursos de SDL_image.
 */
void imagenes_finalizar(void);

#endif // IMAGENES_H_INCLUDED
