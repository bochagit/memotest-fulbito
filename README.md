# TP Tópicos de Programación – Memotest SDL2

Proyecto grupal (4 integrantes)  
Lenguaje: **C**  
Biblioteca gráfica: **SDL2** (+ SDL_image, SDL_ttf, SDL_mixer)

Este repositorio contiene el **código de desarrollo** del Trabajo Práctico.
⚠️ La estructura del ZIP de entrega final será distinta.

---

## 🎯 Objetivo del TP

Desarrollar un videojuego tipo **Memotest** utilizando la biblioteca **SDL2**, cumpliendo con todos los requisitos funcionales, técnicos y formales indicados por la cátedra.

---

## 📋 Checklist general del TP

### 🧱 Estructura y reglas formales (OBLIGATORIO)
- [x] Nombre del grupo válido (RAE + ASCII 0x41–0x5A)
- [ ] Archivo final: `TP_TOPICOS_2025_3C_NOMBREGRUPO.zip`
- [ ] ZIP solo contiene:
  - [ ] `/img`
  - [ ] `/snd`
  - [ ] `/fnt`
  - [ ] `/doc`
  - [ ] Archivos `.c` y `.h`
- [ ] No incluir binarios, proyectos de IDE ni librerías
- [x] `main.c` contiene los datos de **TODOS** los integrantes con formato exacto

---

## ⚙️ Base técnica del proyecto
- [x] Inicialización correcta de SDL2
- [x] Uso de SDL_Renderer (no software rendering)
- [x] Manejo de eventos (mouse, teclado, ventana)
- [x] Liberación correcta de recursos
- [x] Compilación sin warnings

---

## 📦 TDA Vector (REQUISITO EXCLUYENTE)
- [x] Implementar **TDA Vector con memoria dinámica**
- [x] NO usar arrays estáticos para:
  - [x] Cartas
  - [x] Tablero
  - [x] Jugadores
  - [x] Estadísticas
- [x] Funciones mínimas:
  - [x] Crear vector
  - [x] Destruir vector
  - [x] Push
  - [x] Get
  - [x] Remove

---

## 🎮 Funcionalidades mínimas (OBLIGATORIAS)

### Pantallas y flujo
- [x] Pantalla de presentación
- [x] Menú gráfico
- [x] Inicio y cierre correcto del juego

### Juego Memotest
- [x] Tablero configurable:
  - [x] 3x4 (6 pares)
  - [x] 4x4 (8 pares)
  - [x] 4x5 (10 pares)
- [x] Cartas ordenadas aleatoriamente al iniciar
- [x] Cada par tiene puntaje asignado aleatoriamente
- [x] Detección de pares correctos / incorrectos
- [x] Sistema de puntuación
- [x] Sistema de rachas de aciertos
- [x] Estadísticas de juego visibles

---

## 🧑‍💻 Interfaz gráfica (SDL)
- [x] Ingreso de nombre del jugador **desde la interfaz gráfica**
- [x] Visualización de puntaje
- [x] Visualización de estadísticas
- [x] NO usar consola para interacción con el usuario

---

## ⚙️ Configuración persistente
- [x] Menú gráfico de configuración
- [x] Guardar configuración en archivo
- [x] Cargar configuración al iniciar
- [x] Configurar:
  - [x] Dimensiones del tablero
  - [x] Set de figuras
  - [x] Cantidad de jugadores (si aplica)

---

## 🖼️ Recursos gráficos y sonido (RESTRICCIONES)
- [x] Imágenes ≤ 256x256 px
- [x] Imágenes ≤ 200 KB
- [x] Sonidos ≤ 50 KB
- [x] Al menos **2 sets de figuras distintos**

---

## 📄 Documentación (OBLIGATORIA)
- [x] Documentación en `/doc`
- [x] Explica:
  - [x] Cómo instalar SDL2
  - [x] Cómo configurar Code::Blocks
  - [x] Cómo compilar el proyecto
  - [x] Cómo ejecutar el juego
- [x] Pensada para alguien sin experiencia previa
- [x] Útil para la defensa presencial

---

## ⭐ Funcionalidades opcionales (PROMOCIÓN)

- [x] Modo competitivo (2 jugadores)
- [x] Turnos visibles por jugador
- [x] Puntajes individuales
- [x] Ingreso de nombre para cada jugador desde SDL
- [x] Estadísticas por usuario
- [x] Sonidos:
  - [x] Selección primera carta
  - [x] Acierto
  - [x] Error
- [x] Efecto hover al pasar el mouse por una carta

---

## 🚨 Notas importantes

- El repo NO contiene el proyecto de Code::Blocks (`.cbp`)
- Cada integrante configura su propio entorno
- La estructura del ZIP final se arma **al final del desarrollo**
- El TP puede desaprobarse por formato incorrecto aunque el juego funcione
