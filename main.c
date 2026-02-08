/*
Apellido(s), nombre(s): Cardozo, Gonzalo Daniel
DNI: 43777470
Entrega: Sí

Apellido(s), nombre(s): Bruno, Yanil
DNI: 36992182
Entrega: Sí

Apellido(s), nombre(s): De Rogatis, Ramiro Javier
DNI: 44005090
Entrega: Sí

Apellido(s), nombre(s): Sgro, Nicolás Agustín
DNI: 44669267
Entrega: Sí
*/

#include <stdio.h>
#include <stdlib.h>
#include "juego.h"
#include "errores.h"

int main(int argc, char* argv[])
{
    tError err;
    tJuego juego;

    if ((err = juego_inicializar(&juego)) != TODO_OK) {
        fprintf(stderr, "Error: %s\n", errores_obtener_detalle(err));
        return err;
    }

    // Loop principal
    while (juego.corriendo)
    {
        // Procesamiento de eventos (teclado, mouse, eventos de ventana)
        if ((err = juego_procesar_eventos(&juego)) != TODO_OK) {
            fprintf(stderr, "Error: %s\n", errores_obtener_detalle(err));
        }

        // Actualizacion del estado del juego y sus elementos
        juego_actualizar(&juego);

        // Renderizado de la escena
        juego_renderizar(&juego);

        SDL_Delay(LOOP_DELAY); // Esta pausa evita que el procesador sea utilizado al 100%. 16ms limita el renderizado a 60 fps (1000 / 16 = ~62,5fps)
    }

    // Limpieza y liberacion de la memoria
    juego_destruir(&juego);

    return 0;
}
