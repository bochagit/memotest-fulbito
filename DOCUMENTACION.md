# Juego de la Memoria – Documentación

## Descripción
Juego de la memoria implementado en C con SDL2. El jugador debe encontrar
pares de figuras ocultas en un tablero. Incluye:

- Pantalla de presentación con fondo e ingreso de nombre por SDL (sin consola).
- Menú gráfico para configurar dimensiones, set de figuras y cantidad de jugadores.
- Configuración persistente entre sesiones (`config.cfg`).
- Sistema de puntuación con racha de aciertos consecutivos.
- Dos sets de figuras: clubes argentinos y Champions League (imágenes).
- Modo competitivo de 2 jugadores con turnos.
- Sonidos de acierto, fallo y presentación.
- Efecto hover al pasar el mouse sobre las cartas.
- TDA Vector (memoria dinámica) para almacenar cartas y texturas.

---

## Requisitos

| Software         | Versión usada       | Link de descarga                                 |
|------------------|---------------------|--------------------------------------------------|
| CodeBlocks       | 20.03 (con MinGW)   | https://www.codeblocks.org/downloads/             |
| SDL2             | 2.32.8              | https://github.com/libsdl-org/SDL/releases        |
| SDL2_image       | 2.8.8               | https://github.com/libsdl-org/SDL_image/releases  |
| SDL2_mixer       | 2.8.1               | https://github.com/libsdl-org/SDL_mixer/releases  |
| SDL2_ttf         | 2.24.0              | https://github.com/libsdl-org/SDL_ttf/releases    |

Todas las bibliotecas ya están incluidas en la carpeta `lib/` del proyecto.

---

## Estructura de archivos

```
├── main.c              # Punto de entrada
├── juego.c / juego.h   # Módulo principal (loop, init, render)
├── memoria.c / memoria.h   # Lógica del juego de memoria
├── presentacion.c / presentacion.h  # Pantalla de presentación
├── menu.c / menu.h     # Menú gráfico de configuración
├── config.c / config.h # Configuración persistente (config.cfg)
├── vector.c / vector.h # TDA Vector (memoria dinámica)
├── graficos.c / graficos.h  # Motor de renderizado
├── imagenes.c / imagenes.h  # Carga de imágenes (SDL_image)
├── texto.c / texto.h   # Renderizado de texto (SDL_ttf)
├── sonidos.c / sonidos.h    # Audio (SDL_mixer)
├── hud.c / hud.h       # Sistema HUD genérico
├── errores.c / errores.h    # Códigos de error
├── SDL_Ejemplo_Avanzado.cbp  # Proyecto de CodeBlocks
├── config.cfg           # Configuración guardada (se crea automáticamente)
├── img/                 # Imágenes (fondos, figuras de parejas)
├── snd/                 # Sonidos (acierto, fallo, presentación, melodía)
├── fnt/                 # Fuentes TTF
└── lib/                 # Bibliotecas SDL2 precompiladas
```

---

## Instrucciones para compilar en CodeBlocks (Windows)

### 1. Instalar CodeBlocks
Descargar CodeBlocks **con MinGW** desde https://www.codeblocks.org/downloads/
(elegir el instalador que dice "mingw-setup").

### 2. Abrir el proyecto
1. Abrir CodeBlocks.
2. Ir a **File → Open** y seleccionar `SDL_Ejemplo_Avanzado.cbp`.

### 3. Verificar configuración del compilador
El proyecto ya está configurado con las rutas a las bibliotecas SDL2 incluidas
en `lib/`. No es necesario instalar SDL2 por separado.

Si CodeBlocks no encuentra el compilador:
1. Ir a **Settings → Compiler → Toolchain executables**.
2. Verificar que el directorio del compilador apunte a la carpeta `MinGW` 
   dentro de la instalación de CodeBlocks.

### 4. Compilar
- Presionar **F9** (Build and Run) o ir a **Build → Build and run**.
- El ejecutable se genera en `bin/Debug/`.

### 5. Copiar DLLs y assets
Antes de ejecutar, asegurarse de que en la carpeta `bin/Debug/` existan:
- Las carpetas `img/`, `snd/`, `fnt/` con sus contenidos.
- Los archivos `.dll` de SDL2:
  - `SDL2.dll` (de `lib/SDL2-2.32.8/x86_64-w64-mingw32/bin/`)
  - `SDL2_image.dll` (de `lib/SDL2_image-2.8.8/x86_64-w64-mingw32/bin/`)
  - `SDL2_mixer.dll` (de `lib/SDL2_mixer-2.8.1/x86_64-w64-mingw32/bin/`)
  - `SDL2_ttf.dll` (de `lib/SDL2_ttf-2.24.0/x86_64-w64-mingw32/bin/`)
  - Cualquier otra DLL dependiente en esas carpetas `bin/`.

> **Nota:** El directorio de trabajo por defecto de CodeBlocks es la raíz del
> proyecto, por lo que `img/`, `snd/` y `fnt/` ya son accesibles directamente.

### 6. Ejecutar
Al ejecutar se mostrará:
1. **Pantalla de presentación** – fondo con música. Escribir el nombre y pulsar ENTER.
2. **Menú de configuración** – seleccionar dimensiones (3×4, 4×4, 4×5), 
   set de figuras y cantidad de jugadores. Hacer clic en **JUGAR**.
3. **Partida** – hacer clic en las cartas para revelarlas. Encontrar los pares.
4. Al terminar se muestra el resultado. Presionar ESC para salir.

---

## Sistema de puntuación

- Cada par tiene asignado un valor entre 10 y 50 puntos (aleatorio al inicio).
- **Racha de aciertos consecutivos:** cada acierto consecutivo suma un bonus
  del 25% acumulativo:
  - 1er acierto: puntos × 1.0
  - 2do consecutivo: puntos × 1.25
  - 3er consecutivo: puntos × 1.50
  - ...y así sucesivamente.
- Al fallar, la racha se reinicia a 0.

## Modo competitivo (2 jugadores)

- Se turnan para descubrir cartas.
- Tras un acierto, el mismo jugador continúa.
- Tras un fallo, pasa el turno al otro jugador.
- Al finalizar se muestra quién ganó (mayor puntaje).

## Configuración persistente

El archivo `config.cfg` se crea automáticamente con formato:
```
filas=3
columnas=4
set=1
jugadores=1
```
Se carga al iniciar y se guarda al confirmar el menú.

## Controles

| Acción                     | Control             |
|----------------------------|----------------------|
| Seleccionar carta          | Clic izquierdo       |
| Salir del juego            | ESC o cerrar ventana |
| Confirmar nombre           | ENTER                |
| Borrar carácter del nombre | Backspace            |

---

## Requisitos técnicos cumplidos

- [x] Imágenes de figuras ≤ 256×256 px
- [x] Imágenes ≤ 200 KB por archivo
- [x] Sonidos ≤ 50 KB por archivo (verificar en `snd/`)
- [x] TDA Vector implementado con memoria dinámica (`vector.c`)
- [x] Resolución ≤ 1366×768 (ventana de 1024×768)
- [x] Dos sets de figuras
- [x] Nombre solicitado por SDL (no consola)
- [x] Configuración persistente en archivo
