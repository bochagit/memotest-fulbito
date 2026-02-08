

#ifndef ERRORES_H_INCLUDED
#define ERRORES_H_INCLUDED


typedef enum {

    // Generico
    TODO_OK = 0,
    ERR_MEMORIA,

    // SDL
    ERR_SDL,
    ERR_SDL_MIXER,
    ERR_SDL_TTF,
    ERR_SDL_IMAGE,
    ERR_TEXTURA,

    // Audio
    ERR_AUDIO_DRIVER,
    ERR_AUDIO_ASSET,

    // Imagen
    ERR_IMAGEN,

    // HUD
    ERR_HUD_INICIALIZAR,
    ERR_HUD_ACTUALIZAR,

} tError;

/*
  Dado un codigo de error, devuelve una cadena de texto descriptiva.
.
 */
const char* errores_obtener_detalle(tError err);

#endif // ERRORES_H_INCLUDED
