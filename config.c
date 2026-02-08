/**
 * @file config.c
 * @brief Implementación del módulo de configuración persistente.
 */

#include "config.h"
#include <stdio.h>
#include <string.h>

tConfig config_por_defecto(void)
{
    tConfig cfg;
    cfg.filas = 3;
    cfg.columnas = 4;
    cfg.setFiguras = 1;
    cfg.cantJugadores = 1;
    return cfg;
}

tConfig config_cargar(const char *ruta)
{
    tConfig cfg = config_por_defecto();
    FILE *archivo = fopen(ruta, "r");
    if (!archivo) return cfg;

    char linea[128];
    while (fgets(linea, sizeof(linea), archivo)) {
        char clave[64];
        int valor;
        if (sscanf(linea, "%63[^=]=%d", clave, &valor) == 2) {
            if (strcmp(clave, "filas") == 0)          cfg.filas = valor;
            else if (strcmp(clave, "columnas") == 0)   cfg.columnas = valor;
            else if (strcmp(clave, "set") == 0)         cfg.setFiguras = valor;
            else if (strcmp(clave, "jugadores") == 0)   cfg.cantJugadores = valor;
        }
    }
    fclose(archivo);

    /* Validar rangos */
    if (cfg.filas < 3 || cfg.filas > 4) cfg.filas = 3;
    if (cfg.columnas < 4 || cfg.columnas > 5) cfg.columnas = 4;
    if (cfg.setFiguras < 1 || cfg.setFiguras > 2) cfg.setFiguras = 1;
    if (cfg.cantJugadores < 1 || cfg.cantJugadores > 2) cfg.cantJugadores = 1;

    return cfg;
}

int config_guardar(const char *ruta, const tConfig *cfg)
{
    FILE *archivo = fopen(ruta, "w");
    if (!archivo) return -1;

    fprintf(archivo, "filas=%d\n", cfg->filas);
    fprintf(archivo, "columnas=%d\n", cfg->columnas);
    fprintf(archivo, "set=%d\n", cfg->setFiguras);
    fprintf(archivo, "jugadores=%d\n", cfg->cantJugadores);

    fclose(archivo);
    return 0;
}
