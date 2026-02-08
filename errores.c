/**
 * @file errores.c
 * @brief Implementacion del modulo 'errores'.
 */

#include "errores.h"

const char* errores_obtener_detalle(tError err)
{
    switch (err) {
        case TODO_OK:               return "Ejecucion correcta";
        case ERR_MEMORIA:           return "No se pudo asignar memoria RAM";
        case ERR_SDL:               return "Fallo interno de la biblioteca grafica (SDL)";
        case ERR_SDL_MIXER:         return "Fallo en el subsistema de audio (SDL_mixer)";
        case ERR_SDL_TTF:           return "Fallo en el subsistema de fuentes TrueType (SDL_ttf)";
        case ERR_SDL_IMAGE:         return "Fallo en el subsistema de carga de imagenes (SDL_image)";
        case ERR_TEXTURA:           return "No se pudo crear una textura";
        case ERR_AUDIO_DRIVER:      return "No se pudo abrir el dispositivo de audio de su computadora";
        case ERR_AUDIO_ASSET:       return "No se encontro un archivo de audio";
        case ERR_IMAGEN:            return "No se pudo cargar la imagen";
        case ERR_HUD_INICIALIZAR:   return "No se pudo inicializar el modulo HUD";
        case ERR_HUD_ACTUALIZAR:    return "No se pudo actualizar una instancia del HUD";
        default:                    return "Error desconocido";
    }
}
