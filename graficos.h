

#ifndef GRAFICOS_H_INCLUDED
#define GRAFICOS_H_INCLUDED
#include <SDL2/SDL.h>

#define TAM_PIXEL 10
#define PIXELES_X_LADO 12
#define TAM_GRILLA 6
#define PX_PADDING 4

#define N 0 // Negro
#define A 1 // Amarillo
#define B 2 // Blanco
#define V 3 // Verde
#define R 4 // Rojo
#define T 5 // Transparente

/**
 * @brief Dibuja un sprite a partir de una matriz de indices de colores.
 *
 * @param renderer Puntero al renderizador de SDL.
 * @param matriz Matriz del sprite que contiene los indices de la paleta de colores.
 * @param oX Coordenada X en pantalla.
 * @param oY Coordenada Y en pantalla.
 */
void graficos_dibujar(SDL_Renderer *renderer, const uint8_t[][PIXELES_X_LADO], int32_t oX, int32_t oY);

/**
 * @brief Renderizado avanzado de texturas con soporte para transformaciones.
 *
 * @param renderer Puntero al renderizador de SDL.
 * @param textura Puntero a la textura de SDL a dibujar.
 * @param origen Puntero a 'SDL_Rect' que define la porcion de la textura a dibujar, o NULL para toda la textura.
 * @param posX Coordenada X.
 * @param posY Coordenada Y.
 * @param escalaHor Factor de escala horizontal.
 * @param escalaVer Factor de escala vertical.
 * @param angulo Angulo de rotacion en grados.
 * @param flipHor 1 o 0 para activar/desactivar el reflejo horizontal.
 * @param flipVer 1 o 0 para activar/desactivar el reflejo vertical.
 */
void graficos_dibujar_textura(SDL_Renderer *renderer, SDL_Texture *textura, SDL_Rect *origen, int32_t posX, int32_t posY, float escalaHor, float escalaVer, double angulo, uint8_t flipHor, uint8_t flipVer);

/**
 * @brief Limpia el framebuffer actual del renderizador dado un color.
 *
 * @param renderer Puntero al renderizador de SDL.
 * @param color Puntero constante a una estructura 'SDL_Color' con el color a utilizar.
 */
void graficos_borrar_pantalla(SDL_Renderer *renderer, const SDL_Color *color);

/**
 * @brief Crea una textura de tipo framebuffer para renderizado por capas.
 * * * Esta funcion reserva memoria de video para una textura con acceso de tipo
 * 'target' utilizada para composicion de capas.
 *
 * @param renderer Puntero al renderizador de SDL.
 * @param anchoVentana Ancho en pixeles de la textura a crear.
 * @param altoVentana Alto en pixeles de la textura a crear.
 *
 * @return SDL_Texture* Puntero a la textura creada o NULL en caso de error.
 *
 * @note La textura debe ser liberada con la funcion 'SDL_DestroyTexture' al finalizar su uso.
 */
SDL_Texture* graficos_crear_framebuffer(SDL_Renderer *renderer, uint32_t anchoVentana, uint32_t altoVentana);

/**
 * @brief Cambia el framebuffer objetivo de renderizado.
 *
 * @param renderer Puntero al renderizador de SDL.
 * @param target Puntero al framebuffer donde se realizaran las operaciones de dibujado posteriores.
 */
void graficos_cambiar_framebuffer(SDL_Renderer *renderer, SDL_Texture *target);

/**
 * @brief Renderiza multiples framebuffers segun su orden de aparicion.
 * * * Esta funcion realiza la composicion de un array de texturas respetando el orden
 * de aparicion dentro del mismo, para determinar la profundidad (orden Z). De esta forma,
 * el primer elemento del array se dibuja al fondo y el ultimo al frente.
 *
 * @param renderer Puntero al renderizador de SDL.
 * @param framebuffers Puntero a un array de punteros a texturas.
 * @param cantFramebuffers Cantidad total de capas a procesar.
 */
void graficos_renderizar(SDL_Renderer *renderer, SDL_Texture* const *framebuffers, uint32_t cantFramebuffers);


#endif // GRAFICOS_H_INCLUDED
