/**
 * @file menu.c
 * @brief Implementación del menú gráfico de configuración (SDL).
 */

#include "menu.h"
#include "imagenes.h"
#include "highscores.h"
#include "texto.h"
#include <string.h>
#include <stdio.h>

typedef struct {
    SDL_Rect rect;
    const char *texto;
    int seleccionado;
} tOpcionMenu;

/* Dibuja una opción como botón con texto centrado */
static void _dibujar_opcion(SDL_Renderer *renderer, TTF_Font *fuente, tOpcionMenu *op)
{
    if (op->seleccionado)
        SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
    else
        SDL_SetRenderDrawColor(renderer, 100, 100, 130, 255);
    SDL_RenderFillRect(renderer, &op->rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &op->rect);

    SDL_Texture *tex = texto_crear_textura(renderer, fuente, op->texto,
                                           (SDL_Color){255,255,255,255});
    if (tex) {
        int w, h;
        SDL_QueryTexture(tex, NULL, NULL, &w, &h);
        /* Limitar al ancho del botón */
        if (w > op->rect.w - 8) w = op->rect.w - 8;
        if (h > op->rect.h - 4) h = op->rect.h - 4;
        SDL_Rect dst = {
            op->rect.x + (op->rect.w - w) / 2,
            op->rect.y + (op->rect.h - h) / 2,
            w, h
        };
        SDL_RenderCopy(renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
}

/* Devuelve 1 si (mx,my) está dentro de rect */
static int _dentro(const SDL_Rect *r, int mx, int my)
{
    return mx >= r->x && mx <= r->x + r->w &&
           my >= r->y && my <= r->y + r->h;
}

/* ---- Función pública ---- */

tAccionMenu menu_mostrar(SDL_Renderer *renderer, TTF_Font *fuente, const char *fondoPath, tConfig *cfg, char *nombreJugador2, size_t maxLen)
{
    if (!renderer || !fuente || !cfg) return ERR_MEMORIA;

    int anchoV, altoV;
    SDL_GetRendererOutputSize(renderer, &anchoV, &altoV);
    int centroX = anchoV / 2;

    SDL_Texture *fondo = imagenes_cargar_gpu(renderer, fondoPath);

    /* ---- Definir opciones ---- */
    int anchoBtn = 150;
    int altoBtn  = 50;
    int espH     = 15;

    /* Dimensiones: 3x4, 4x4, 4x5 */
    int dimY = 160;
    tOpcionMenu dimOpc[3];
    dimOpc[0] = (tOpcionMenu){ {centroX - anchoBtn - anchoBtn/2 - espH, dimY, anchoBtn, altoBtn}, "3 x 4", 0 };
    dimOpc[1] = (tOpcionMenu){ {centroX - anchoBtn/2, dimY, anchoBtn, altoBtn},                   "4 x 4", 0 };
    dimOpc[2] = (tOpcionMenu){ {centroX + anchoBtn/2 + espH, dimY, anchoBtn, altoBtn},             "4 x 5", 0 };

    /* Set de figuras */
    int setY = dimY + altoBtn + 80;
    tOpcionMenu setOpc[2];
    setOpc[0] = (tOpcionMenu){ {centroX - anchoBtn - espH/2, setY, anchoBtn, altoBtn}, "Clubes AR", 0 };
    setOpc[1] = (tOpcionMenu){ {centroX + espH/2,            setY, anchoBtn, altoBtn}, "Champions", 0 };

    /* Jugadores */
    int jugY = setY + altoBtn + 80;
    tOpcionMenu jugOpc[2];
    jugOpc[0] = (tOpcionMenu){ {centroX - anchoBtn - espH/2, jugY, anchoBtn, altoBtn}, "1 Jugador",   0 };
    jugOpc[1] = (tOpcionMenu){ {centroX + espH/2,            jugY, anchoBtn, altoBtn}, "2 Jugadores", 0 };

    /* Botón JUGAR */
    int jugarY = jugY + altoBtn + 90;
    SDL_Rect botonJugar = { centroX - 210, jugarY, 200, 60 }; // Movido a la izquierda
    SDL_Rect botonScores = { centroX + 10, jugarY, 200, 60 }; // Botón nuevo a la derecha

    /* Selección inicial según config */
    if      (cfg->filas == 4 && cfg->columnas == 5) dimOpc[2].seleccionado = 1;
    else if (cfg->filas == 4 && cfg->columnas == 4) dimOpc[1].seleccionado = 1;
    else                                            dimOpc[0].seleccionado = 1;

    setOpc[cfg->setFiguras == 2 ? 1 : 0].seleccionado = 1;
    jugOpc[cfg->cantJugadores == 2 ? 1 : 0].seleccionado = 1;

    /* ---- Loop del menú ---- */
    while (1) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) return ACCION_SALIR;
            if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT) {
                int mx = ev.button.x, my = ev.button.y;

                for (int i = 0; i < 3; ++i)
                    if (_dentro(&dimOpc[i].rect, mx, my)) {
                        for (int j = 0; j < 3; ++j) dimOpc[j].seleccionado = 0;
                        dimOpc[i].seleccionado = 1;
                    }
                for (int i = 0; i < 2; ++i)
                    if (_dentro(&setOpc[i].rect, mx, my)) {
                        for (int j = 0; j < 2; ++j) setOpc[j].seleccionado = 0;
                        setOpc[i].seleccionado = 1;
                    }
                for (int i = 0; i < 2; ++i)
                    if (_dentro(&jugOpc[i].rect, mx, my)) {
                        for (int j = 0; j < 2; ++j) jugOpc[j].seleccionado = 0;
                        jugOpc[i].seleccionado = 1;
                    }
                if (_dentro(&botonJugar, mx, my)) return ACCION_JUGAR;
                if (_dentro(&botonScores, mx, my)) return ACCION_VER_SCORES;
            }
        }

        /* ---- Render ---- */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (fondo) {
            SDL_RenderCopy(renderer, fondo, NULL, NULL);
        }

        /* Título */
        SDL_Texture *tTitulo = texto_crear_textura(renderer, fuente,
            "MEMOTEST - FULBITO", (SDL_Color){255, 255, 100, 255});
        if (tTitulo) {
            int w, h; SDL_QueryTexture(tTitulo, NULL, NULL, &w, &h);
            SDL_Rect dst = { centroX - w/2, 30, w, h };
            SDL_RenderCopy(renderer, tTitulo, NULL, &dst);
            SDL_DestroyTexture(tTitulo);
        }

        /* Etiquetas */
        const char *etiquetas[] = { "Dimensiones:", "Set de figuras:", "Jugadores:" };
        int etiquetasY[]        = { dimY - 40, setY - 40, jugY - 40 };
        for (int i = 0; i < 3; ++i) {
            SDL_Texture *tLbl = texto_crear_textura(renderer, fuente,
                etiquetas[i], (SDL_Color){255,255,255,255});
            if (tLbl) {
                int w, h; SDL_QueryTexture(tLbl, NULL, NULL, &w, &h);
                SDL_Rect dst = { centroX - w/2, etiquetasY[i], w, h };
                SDL_RenderCopy(renderer, tLbl, NULL, &dst);
                SDL_DestroyTexture(tLbl);
            }
        }

        /* Opciones */
        for (int i = 0; i < 3; ++i) _dibujar_opcion(renderer, fuente, &dimOpc[i]);
        for (int i = 0; i < 2; ++i) _dibujar_opcion(renderer, fuente, &setOpc[i]);
        for (int i = 0; i < 2; ++i) _dibujar_opcion(renderer, fuente, &jugOpc[i]);

        /* Botón JUGAR */
        SDL_SetRenderDrawColor(renderer, 200, 60, 60, 255);
        SDL_RenderFillRect(renderer, &botonJugar);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &botonJugar);
        SDL_Texture *tJugar = texto_crear_textura(renderer, fuente, "JUGAR", (SDL_Color){255,255,255,255});
        if (tJugar) {
            int w, h; SDL_QueryTexture(tJugar, NULL, NULL, &w, &h);
            SDL_Rect dst = { botonJugar.x + (botonJugar.w-w)/2, botonJugar.y + (botonJugar.h-h)/2, w, h };
            SDL_RenderCopy(renderer, tJugar, NULL, &dst);
            SDL_DestroyTexture(tJugar);
        }

        /* Botón SCORES */
        SDL_SetRenderDrawColor(renderer, 60, 60, 200, 255); // Color azulado
        SDL_RenderFillRect(renderer, &botonScores);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &botonScores);
        SDL_Texture *tScores = texto_crear_textura(renderer, fuente, "SCORES", (SDL_Color){255,255,255,255});
        if (tScores) {
            int w, h; SDL_QueryTexture(tScores, NULL, NULL, &w, &h);
            SDL_Rect dst = { botonScores.x + (botonScores.w-w)/2, botonScores.y + (botonScores.h-h)/2, w, h };
            SDL_RenderCopy(renderer, tScores, NULL, &dst);
            SDL_DestroyTexture(tScores);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    /* ---- Aplicar selecciones ---- */
    if      (dimOpc[0].seleccionado) { cfg->filas = 3; cfg->columnas = 4; }
    else if (dimOpc[1].seleccionado) { cfg->filas = 4; cfg->columnas = 4; }
    else                             { cfg->filas = 4; cfg->columnas = 5; }

    cfg->setFiguras   = setOpc[1].seleccionado ? 2 : 1;
    cfg->cantJugadores = jugOpc[1].seleccionado ? 2 : 1;

    /* ---- Si hay 2 jugadores, pedir nombre del segundo ---- */
    if (cfg->cantJugadores == 2 && nombreJugador2 && maxLen > 0) {
        char buffer[256] = {0};
        size_t len = 0;
        SDL_StartTextInput();
        int terminado = 0;
        uint32_t ultimoBlink = SDL_GetTicks();
        int cursorVisible = 1;

        while (!terminado) {
            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT) { SDL_StopTextInput(); return ERR_SDL; }
                if (ev.type == SDL_TEXTINPUT) {
                    size_t agregar = strlen(ev.text.text);
                    if (len + agregar < sizeof(buffer) && len + agregar < maxLen) {
                        memcpy(buffer + len, ev.text.text, agregar);
                        len += agregar;
                        buffer[len] = '\0';
                    }
                }
                if (ev.type == SDL_KEYDOWN) {
                    if (ev.key.keysym.sym == SDLK_BACKSPACE && len > 0) {
                        len--;
                        buffer[len] = '\0';
                    }
                    if (ev.key.keysym.sym == SDLK_RETURN ||
                        ev.key.keysym.sym == SDLK_KP_ENTER) {
                        terminado = 1;
                    }
                }
            }

            uint32_t ahora = SDL_GetTicks();
            if (ahora - ultimoBlink >= 500) {
                cursorVisible = !cursorVisible;
                ultimoBlink = ahora;
            }

            SDL_Color blanco = {255,255,255,255};
            SDL_Texture *tInst = texto_crear_textura(renderer, fuente,
                "Nombre del Jugador 2 (ENTER para continuar):", blanco);
            if (tInst) {
                int w, h; SDL_QueryTexture(tInst, NULL, NULL, &w, &h);
                SDL_Rect dst = { centroX - w/2, altoV/2 - 60, w, h };
                SDL_RenderCopy(renderer, tInst, NULL, &dst);
                SDL_DestroyTexture(tInst);
            }

            char mostrar[300];
            snprintf(mostrar, sizeof(mostrar), "%s%s", buffer, cursorVisible ? "|" : " ");
            SDL_Texture *tNom = texto_crear_textura(renderer, fuente, mostrar, blanco);
            if (tNom) {
                int w, h; SDL_QueryTexture(tNom, NULL, NULL, &w, &h);
                SDL_Rect dst = { centroX - w/2, altoV/2, w, h };
                SDL_RenderCopy(renderer, tNom, NULL, &dst);
                SDL_DestroyTexture(tNom);
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
        SDL_StopTextInput();
        strncpy(nombreJugador2, buffer, maxLen - 1);
        nombreJugador2[maxLen - 1] = '\0';
    }

        if (fondo) {
        SDL_DestroyTexture(fondo);
    }

    return TODO_OK;
}

void menu_mostrar_highscores(SDL_Renderer *renderer, TTF_Font *fuente, const char *fondoPath) {
    tEntradaScore lista[MAX_TOP];
    int cant = highscores_cargar(lista);
    SDL_Texture *fondo = imagenes_cargar_gpu(renderer, fondoPath);

    int anchoV, altoV;
    SDL_GetRendererOutputSize(renderer, &anchoV, &altoV);
    int centroX = anchoV / 2;

    int salir = 0;
    while (!salir) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            // Si cierran la ventana, salimos del programa
            if (ev.type == SDL_QUIT) exit(0);
            // Cualquier tecla o clic vuelve al menú principal
            if (ev.type == SDL_KEYDOWN || ev.type == SDL_MOUSEBUTTONDOWN) salir = 1;
        }

        SDL_RenderClear(renderer);
        if (fondo) SDL_RenderCopy(renderer, fondo, NULL, NULL);

        /* Título */
        SDL_Texture *tTit = texto_crear_textura(renderer, fuente, "TOP 10 MEJORES PUNTAJES", (SDL_Color){255, 255, 100, 255});
        if (tTit) {
            int w, h; SDL_QueryTexture(tTit, NULL, NULL, &w, &h);
            SDL_Rect dst = { centroX - w/2, 50, w, h };
            SDL_RenderCopy(renderer, tTit, NULL, &dst);
            SDL_DestroyTexture(tTit);
        }

        /* Lista de puntajes */
        for (int i = 0; i < cant; i++) {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "%d. %s - %d Pts (%dx%d)",
                     i+1, lista[i].nombre, lista[i].puntos, lista[i].filas, lista[i].columnas);

            SDL_Texture *tLin = texto_crear_textura(renderer, fuente, buffer, (SDL_Color){255, 255, 255, 255});
            if (tLin) {
                int w, h; SDL_QueryTexture(tLin, NULL, NULL, &w, &h);
                // Ajustamos el Y para que cada línea baje 35 píxeles
                SDL_Rect dst = { centroX - w/2, 120 + i * 35, w, h };
                SDL_RenderCopy(renderer, tLin, NULL, &dst);
                SDL_DestroyTexture(tLin);
            }
        }

        /* Mensaje al pie */
        SDL_Texture *tVol = texto_crear_textura(renderer, fuente, "Click o Tecla para volver", (SDL_Color){180, 180, 180, 255});
        if (tVol) {
            int w, h; SDL_QueryTexture(tVol, NULL, NULL, &w, &h);
            SDL_Rect dst = { centroX - w/2, altoV - 60, w, h };
            SDL_RenderCopy(renderer, tVol, NULL, &dst);
            SDL_DestroyTexture(tVol);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (fondo) SDL_DestroyTexture(fondo);
}
