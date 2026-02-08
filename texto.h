
#ifndef SDL_EJEMPLO_TEXTO_H
#define SDL_EJEMPLO_TEXTO_H
#include "errores.h"
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/**
 * @brief Inicializa el subsistema SDL_ttf.
 *
 * @return tError TODO_OK si la inicialización fue exitosa, o ERR_SDL_TTF en caso de falla.
 */
tError texto_inicializar(void);

/**
 * @brief Carga una fuente TrueType (.ttf) desde el disco.
 * @param path Ruta al archivo.
 * @param tam Tamano de la fuente en puntos.
 * @return TTF_Font* Puntero a la fuente cargada o NULL si hubo error.
 */
TTF_Font* texto_cargar_fuente(const char *path, uint32_t tam);

/**
 * @brief Libera la memoria asociada a la fuente.
 * @param fuente Puntero a la fuente a destruir.
 */
void texto_destruir_fuente(TTF_Font *fuente);

/**
 * @brief Genera una textura a partir de una cadena de texto.
 *
 * @param renderer Puntero al renderizador de SDL.
 * @param fuente Puntero a una fuente TTF.
 * @param texto Cadena con la que se generara la textura.
 * @param color Estructura 'SDL_Color' con el color del texto.
 *
 * @return SDL_Texture* Puntero a la textura generada, o NULL en caso de error.
 *
 * @note La textura debe ser liberada con la funcion 'SDL_DestroyTexture' al finalizar su uso.
 */
SDL_Texture* texto_crear_textura(SDL_Renderer *renderer, TTF_Font *fuente, const char* texto, SDL_Color color);

/**
 * @brief Libera los recursos de SDL_ttf.
 */
void texto_finalizar(void);

#endif
