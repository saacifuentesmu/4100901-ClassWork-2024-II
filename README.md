# Emulador de Luces de Parqueo y Direccionales en un STM32 (Nucleo)

Este documento describe una arquitectura de referencia para emular el comportamiento de las luces de parqueo (hazard) y direccionales en una placa STM32 (por ejemplo, Nucleo). Se busca que la solución sea mantenible, escalable y fácil de entender, sirviendo como guía para estudiantes que estén cursando una asignatura de estructuras computacionales orientada al desarrollo de firmware embebido.

## Objetivos

- **Luces de Parqueo (Hazard):** Las luces parpadean de forma continua mientras el modo hazard está activado. Se activa y desactiva con un evento (por ejemplo, un botón o un comando UART).
  
- **Direccionales con Comportamiento Inteligente:**
  - Una pulsación del botón de direccional (izquierda o derecha) hace que la luz parpadee 3 veces.
  - Dos pulsaciones consecutivas (en un intervalo de 300ms) hacen que la direccional parpadee indefinidamente hasta que se reciba el comando de detener.
  
Este comportamiento se asemeja a sistemas de ayuda de giro en algunos vehículos modernos como el [Tesla Model 3](https://www.youtube.com/watch?v=3wZVLvbsBrc), donde una pulsación corta de la palanca de direccionales produce un parpadeo corto (3 veces) y una pulsación más firme inicia el parpadeo continuo.

## Arquitectura Propuesta

La arquitectura se basa en una división por capas y en la implementación de una Máquina de Estados Finita (FSM):

1. **Capa de Abstracción de Hardware (HAL):**
   - `gpio.[c/h]`: Configuración de pines, funciones primitivas para encender/apagar LEDs y leer botones.
   - `uart.[c/h]`: Configuración y funciones de envío/recepción de datos por UART.
   - `systick.[c/h]`: Uso del SysTick para contar milisegundos y proporcionar temporización.

2. **Capa de Drivers Específicos:**
   - `led_driver.[c/h]`: Funciones para realizar patrones de parpadeo. Por ejemplo, `led_driver_toggle_left()`, `led_driver_toggle_right()`, `led_driver_toggle_both()`, `led_driver_all_off()`.
   - `button_driver.[c/h]`: Lectura de estados de los botones de direccionales y hazard, detección de pulsaciones simples y dobles (tiempo entre pulsaciones), y antirrebote.
   - `commands.[c/h]` (opcional): Interpretar comandos UART para activar/desactivar hazard o detener el parpadeo infinito.

3. **Capa de Lógica de Aplicación (FSM):**
   - `parking_lights.[c/h]`: Contiene la lógica principal. Define los estados, las transiciones y la lógica temporal:
     - **Estados Ejemplo:**
       - `STATE_OFF`: Sin luces encendidas.
       - `STATE_HAZARD`: Parpadeo continuo de las luces.
       - `STATE_TURN_LEFT_3BLINKS`: Parpadeo 3 veces del lado izquierdo.
       - `STATE_TURN_LEFT_INFINITE`: Parpadeo indefinido del lado izquierdo.
       - `STATE_TURN_RIGHT_3BLINKS`: Parpadeo 3 veces del lado derecho.
       - `STATE_TURN_RIGHT_INFINITE`: Parpadeo indefinido del lado derecho.
     
     Cada estado ajusta las salidas (LEDs) y depende de eventos (botones, UART, tiempo) para transicionar. Por ejemplo, si se detecta una pulsación doble en el botón izquierdo estando en `STATE_OFF`, se transita a `STATE_TURN_LEFT_INFINITE`. Para detener este estado, un comando o una pulsación distinta moverá el estado de vuelta a `STATE_OFF`.

## Ejemplo Simplificado de la Máquina de Estados

```c
typedef enum {
    STATE_OFF,
    STATE_HAZARD,
    STATE_TURN_LEFT_3BLINKS,
    STATE_TURN_LEFT_INFINITE,
    STATE_TURN_RIGHT_3BLINKS,
    STATE_TURN_RIGHT_INFINITE
} parking_light_state_t;
```

### Transiciones Ejemplo:

```
OFF + botón hazard → HAZARD
OFF + botón izq (1 pulsación) → TURN_LEFT_3BLINKS
OFF + botón izq (2 pulsaciones rápidas) → TURN_LEFT_INFINITE
TURN_LEFT_3BLINKS → al cumplir 3 parpadeos → OFF
TURN_LEFT_INFINITE + comando "stop" → OFF
```

### Lógica de Tiempo:

* El SysTick o un timer sirve para contar milisegundos. Por ejemplo, cada 500 ms se alterna el estado de la luz (encendido/apagado) para generar el parpadeo.
* Para los patrones de 3 parpadeos, se lleva un conteo interno (ej: cada toggle cuenta como medio ciclo de parpadeo; 6 toggles = 3 parpadeos on/off).
