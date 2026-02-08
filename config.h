

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

typedef struct {
    int filas;           // 3 o 4
    int columnas;        // 4 o 5
    int setFiguras;      // 1 o 2
    int cantJugadores;   // 1 o 2
} tConfig;

/* Devuelve configuración por defecto (3x4, set 1, 1 jugador). */
tConfig config_por_defecto(void);

/* Carga configuración desde archivo. Si no existe, devuelve valores por defecto. */
tConfig config_cargar(const char *ruta);

/* Guarda configuración en archivo. Retorna 0 si OK, -1 si error. */
int config_guardar(const char *ruta, const tConfig *cfg);

#endif // CONFIG_H_INCLUDED
