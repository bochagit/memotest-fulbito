#include "highscores.h"
#include "imagenes.h"
#include <string.h>

int highscores_cargar(tEntradaScore scores[]) {
    FILE *archivo = fopen(RUTA_HIGHSCORES, "r");
    if (!archivo) return 0;

    int i = 0;
    // Formato: Nombre,Puntos,Filas,Columnas
    while (i < MAX_TOP && fscanf(archivo, "%49[^,],%d,%d,%d\n",
           scores[i].nombre, &scores[i].puntos, &scores[i].filas, &scores[i].columnas) == 4) {
        i++;
    }

    fclose(archivo);
    return i;
}

void highscores_insertar(const char *nombre, int puntos, int filas, int columnas) {
    tEntradaScore scores[MAX_TOP + 1]; // Espacio para los 10 actuales + el nuevo
    int cant = highscores_cargar(scores);

    // 1. Agregar el nuevo puntaje al final del array
    strncpy(scores[cant].nombre, nombre, MAX_NOMBRE_HS - 1);
    scores[cant].nombre[MAX_NOMBRE_HS - 1] = '\0'; // Asegurar cierre de cadena
    scores[cant].puntos = puntos;
    scores[cant].filas = filas;
    scores[cant].columnas = columnas;
    cant++;

    // 2. Ordenar de mayor a menor (Bubble Sort)
    for (int i = 0; i < cant - 1; i++) {
        for (int j = 0; j < cant - i - 1; j++) {
            if (scores[j].puntos < scores[j+1].puntos) {
                tEntradaScore temp = scores[j];
                scores[j] = scores[j+1];
                scores[j+1] = temp;
            }
        }
    }

    // 3. Si nos pasamos del Top 10, truncamos la lista
    if (cant > MAX_TOP) {
        cant = MAX_TOP;
    }

    // 4. Guardar la lista actualizada en el archivo
    FILE *archivo = fopen(RUTA_HIGHSCORES, "w");
    if (!archivo) return;

    for (int i = 0; i < cant; i++) {
        fprintf(archivo, "%s,%d,%d,%d\n",
                scores[i].nombre, scores[i].puntos, scores[i].filas, scores[i].columnas);
    }

    fclose(archivo);
}

