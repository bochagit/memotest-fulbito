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
- [ ] Nombre del grupo válido (RAE + ASCII 0x41–0x5A)
- [ ] Archivo final: `TP_TOPICOS_2025_3C_NOMBREGRUPO.zip`
- [ ] ZIP solo contiene:
  - [ ] `/img`
  - [ ] `/snd`
  - [ ] `/fnt`
  - [ ] `/doc`
  - [ ] Archivos `.c` y `.h`
- [ ] No incluir binarios, proyectos de IDE ni librerías
- [ ] `main.c` contiene los datos de **TODOS** los integrantes con formato exacto

---

## ⚙️ Base técnica del proyecto
- [ ] Inicialización correcta de SDL2
- [ ] Uso de SDL_Renderer (no software rendering)
- [ ] Manejo de eventos (mouse, teclado, ventana)
- [ ] Liberación correcta de recursos
- [ ] Compilación sin warnings

---

## 📦 TDA Vector (REQUISITO EXCLUYENTE)
- [ ] Implementar **TDA Vector con memoria dinámica**
- [ ] NO usar arrays estáticos para:
  - [ ] Cartas
  - [ ] Tablero
  - [ ] Jugadores
  - [ ] Estadísticas
- [ ] Funciones mínimas:
  - [ ] Crear vector
  - [ ] Destruir vector
  - [ ] Push
  - [ ] Get
  - [ ] Remove

---

## 🎮 Funcionalidades mínimas (OBLIGATORIAS)

### Pantallas y flujo
- [ ] Pantalla de presentación
- [ ] Menú gráfico
- [ ] Inicio y cierre correcto del juego

### Juego Memotest
- [ ] Tablero configurable:
  - [ ] 3x4 (6 pares)
  - [ ] 4x4 (8 pares)
  - [ ] 4x5 (10 pares)
- [ ] Cartas ordenadas aleatoriamente al iniciar
- [ ] Cada par tiene puntaje asignado aleatoriamente
- [ ] Detección de pares correctos / incorrectos
- [ ] Sistema de puntuación
- [ ] Sistema de rachas de aciertos
- [ ] Estadísticas de juego visibles

---

## 🧑‍💻 Interfaz gráfica (SDL)
- [ ] Ingreso de nombre del jugador **desde la interfaz gráfica**
- [ ] Visualización de puntaje
- [ ] Visualización de estadísticas
- [ ] NO usar consola para interacción con el usuario

---

## ⚙️ Configuración persistente
- [ ] Menú gráfico de configuración
- [ ] Guardar configuración en archivo
- [ ] Cargar configuración al iniciar
- [ ] Configurar:
  - [ ] Dimensiones del tablero
  - [ ] Set de figuras
  - [ ] Cantidad de jugadores (si aplica)

---

## 🖼️ Recursos gráficos y sonido (RESTRICCIONES)
- [ ] Imágenes ≤ 256x256 px
- [ ] Imágenes ≤ 200 KB
- [ ] Sonidos ≤ 50 KB
- [ ] Al menos **2 sets de figuras distintos**

---

## 📄 Documentación (OBLIGATORIA)
- [ ] Documentación en `/doc`
- [ ] Explica:
  - [ ] Cómo instalar SDL2
  - [ ] Cómo configurar Code::Blocks
  - [ ] Cómo compilar el proyecto
  - [ ] Cómo ejecutar el juego
- [ ] Pensada para alguien sin experiencia previa
- [ ] Útil para la defensa presencial

---

## ⭐ Funcionalidades opcionales (PROMOCIÓN)

- [ ] Modo competitivo (2 jugadores)
- [ ] Turnos visibles por jugador
- [ ] Puntajes individuales
- [ ] Ingreso de nombre para cada jugador desde SDL
- [ ] Estadísticas por usuario
- [ ] Sonidos:
  - [ ] Selección primera carta
  - [ ] Acierto
  - [ ] Error
- [ ] Efecto hover al pasar el mouse por una carta

---

## 🚨 Notas importantes

- El repo NO contiene el proyecto de Code::Blocks (`.cbp`)
- Cada integrante configura su propio entorno
- La estructura del ZIP final se arma **al final del desarrollo**
- El TP puede desaprobarse por formato incorrecto aunque el juego funcione
