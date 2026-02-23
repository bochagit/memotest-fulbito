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
#include "menu.h"
#include "presentacion.h"

int main(int argc, char* argv[])
{
    tError err;
    tJuego juego;

    if ((err = juego_inicializar(&juego)) != TODO_OK)
    {
        fprintf(stderr, "Error: %s\n", errores_obtener_detalle(err));
        return err;
    }

    // Loop principal
    while (juego.corriendo)
    {
        tAccionMenu accion = juego_procesar_eventos(&juego);

        if (accion == ACCION_VOLVER_MENU)
        {
            if (juego.partida) {
                memoria_destruir(juego.partida);
                juego.partida = NULL;
            }
            if (juego.ranking) {
                vector_destroy(juego.ranking);
                juego.ranking = NULL;
            }
            juego.rankingGuardado = 0;

            int navegando = 1;
            while (navegando) {
                tAccionMenu accionMenu = menu_mostrar(juego.renderer,
                                                     juego.fuenteChica,
                                                     "img/fondo_presentacion.png",
                                                     &juego.configuracion,
                                                     juego.nombreJugador1,
                                                     juego.nombreJugador2,
                                                     sizeof(juego.nombreJugador1),
                                                     sizeof(juego.nombreJugador2));

                if (accionMenu == ACCION_VER_SCORES) {
                    menu_mostrar_highscores(juego.renderer, juego.fuenteChica, "img/fondo_presentacion.png");
                }
                else if (accionMenu == ACCION_CAMBIAR_NOMBRES) {
                    // Cambiar ambos nombres
                    presentacion_mostrar(juego.renderer, juego.fuenteGrande,
                                       "img/fondo_presentacion.png",
                                       "snd/Sonido_presentacion.mp3",
                                       "Ingrese el nombre de jugador 1",
                                       juego.nombreJugador1,
                                       sizeof(juego.nombreJugador1));
                    
                    presentacion_mostrar(juego.renderer, juego.fuenteGrande,
                                       "img/fondo_presentacion.png",
                                       "snd/Sonido_presentacion.mp3",
                                       "Ingrese el nombre de jugador 2",
                                       juego.nombreJugador2,
                                       sizeof(juego.nombreJugador2));
                }
                else if (accionMenu == ACCION_JUGAR) {
                    /* Si eligió 2 jugadores y no tiene nombre el jugador 2, pedirlo */
                    if (juego.configuracion.cantJugadores == 2 && !juego.nombreJugador2[0]) {
                        presentacion_mostrar(juego.renderer, juego.fuenteGrande,
                                           "img/fondo_presentacion.png",
                                           "snd/Sonido_presentacion.mp3",
                                           "Ingrese el nombre de jugador 2",
                                           juego.nombreJugador2,
                                           sizeof(juego.nombreJugador2));
                    }
                    navegando = 0;
                }
                else {
                    juego.corriendo = 0;
                    navegando = 0;
                    break;
                }
            }

            if (!juego.corriendo) break;

            config_guardar(RUTA_CONFIG, &juego.configuracion);

            juego.partida = memoria_crear(juego.renderer,
                                         juego.configuracion.filas,
                                         juego.configuracion.columnas,
                                         juego.configuracion.setFiguras,
                                         juego.audioInicializado,
                                         juego.configuracion.cantJugadores);
            if (!juego.partida) {
                fprintf(stderr, "Error al crear la partida.\n");
                juego.corriendo = 0;
            }
        }
        else if (accion == ACCION_SALIR)
        {
            juego.corriendo = 0;
        }

        juego_actualizar(&juego);
        juego_renderizar(&juego);
        SDL_Delay(LOOP_DELAY);
    }

    juego_destruir(&juego);
    return 0;
}