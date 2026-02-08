
#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED
#include "errores.h"
#include <stdint.h>
#include <SDL2/SDL.h>

/**
 * @brief Estructura opaca para el manejo del HUD.
 * * * Mediante el uso de una estructura opaca, los campos internos solo son
 * accesibles y modificables mediante las funciones expuestas por la API,
 * protegiendo y encapsulando el estado del elemento.
 * * * El siguiente 'typedef' es una declaracion adelantada que le indica al
 * compilador la existencia del tipo 'struct sHUD'. El usuario que desee integrar
 * esta API estara restringido al uso exclusivo de punteros del tipo 'tHUD' y
 * su respectiva interfaz de funciones expuesta.
 */
typedef struct sHUD tHUD;


/**********************
 * CALLBACKS DE LA API
 **********************/

/**
 * @brief Firma de la funcion para la actualizacion de la textura del HUD.
 * * * Este callback permite que quien utiliza la API determine se debe actualizar
 * la textura de la instancia.
 *
 * @param renderer Puntero al renderizador de SDL necesario para crear texturas asociadas al mismo.
 * @param dato Puntero al dato actual almacenado en la estructura del HUD.
 * @param datoNuevo Puntero con el dato actualizado que el usuario necesite.
 * @param extra Puntero a recursos externos necesarios para la actualizacion.
 *
 * @return SDL_Texture* La nueva textura generada para la instancia.
 */
typedef SDL_Texture* (*tActualizarHUD) (SDL_Renderer *renderer, void *dato, void *datoNuevo, void *extra);

/**
 * @brief Firma de la funcion para la liberacion de recursos externos.
 * * * Si el HUD recibio un puntero 'extra' que queda asociado permanentemente a esta instancia
 * esta funcion se encarga de liberarlo al destruirse la instancia del HUD.
 *
 * @param extra Puntero al recurso que debe ser liberado.
 */
typedef void (*tDestruirHUD) (void *extra);



/**********************
 * FUNCIONES DE LA API
 **********************/

/**
 * @brief Crea e inicializa una instancia del HUD
 * Reserva memoria para la estructura y copia los datos iniciales.
 *
 * @param renderer Puntero al renderizador de SDL.
 * @param posX Coordenada X inicial.
 * @param posY Coordenada Y inicial.
 * @param dato Puntero a los datos iniciales del HUD.
 * @param tamDato Bytes del dato.
 * @param extra Puntero opcional para datos adicionales externos.
 * @param actualizar Callback para la actualizacion de la textura.
 * @param destruir Callback para liberar recursos externos.
 *
 * @return tHUD* Puntero a la instancia creada, o NULL si fallo la creacion.
 */
tHUD* hud_inicializar (SDL_Renderer *renderer, int32_t posX, int32_t posY, void *dato, size_t tamDato, void *extra, tActualizarHUD actualizar, tDestruirHUD destruir);

/**
 * @brief Renderiza la textura del HUD.
 *
 * @param hud Puntero constante a la instancia del HUD.
 */
void hud_dibujar (const tHUD *hud);

/**
 * @brief Actualiza el contenido del HUD.
 * * * Ejecuta el callback 'actualizar' y reemplaza la textura anterior por la nueva.
 *
 * @param hud Puntero a la instancia del HUD.
 * @param datoNuevo Puntero al nuevo dato a visualizar.
 *
 * @return tError TODO_OK si se actualizo correctamente, o un codigo de error.
 */
tError hud_actualizar_dato (tHUD *hud, void *datoNuevo);

/**
 * @brief Define la nueva posicion y angulo del HUD de forma absoluta.
 *
 * @param hud Puntero a la instancia del HUD.
 * @param posX Nueva coordenada X.
 * @param posY Nueva coordenada Y.
 * @param angulo Nuevo angulo de rotacion en grados.
 */
void hud_actualizar_posicion_abs (tHUD *hud, int32_t posX, int32_t posY, double angulo);

/**
 * @brief Define la nueva posicion y angulo del HUD de forma relativa.
 *
 * @param hud Puntero a la instancia del HUD.
 * @param posX Suma a la coordenada X.
 * @param posY Suma a la coordenada Y.
 * @param angulo Suma al angulo de rotacion en grados.
 */
void hud_actualizar_posicion_rel (tHUD *hud, int32_t posX, int32_t posY, double angulo);

/**
 * @brief Libera los recursos de la instancia del HUD.
 *
 * @param hud Puntero a la instancia del HUD a destruir.
 */
void hud_destruir(tHUD *hud);

#endif // HUD_H_INCLUDED
