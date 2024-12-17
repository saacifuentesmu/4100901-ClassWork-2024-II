#include "button_driver.h"
#include "gpio.h"
#include "systick.h"
#include "parking_lights.h"

static uint32_t last_press_time_left = 0;
static uint32_t last_press_time_right = 0;
static uint8_t press_count_left = 0;
static uint8_t press_count_right = 0;

void button_init(void) {
    init_gpio_pin(GPIOA, 0, GPIO_INPUD); // Botón izquierdo
    init_gpio_pin(GPIOA, 1, GPIO_INPUD); // Botón derecho
}

ButtonEvent_t button_read_event(uint8_t button_id) {
    uint32_t current_time = systick_GetTick();
    if (button_id == 0) {
        if (gpio_read_pin(GPIOA, 0)) {
            if (current_time - last_press_time_left < 200) {
                press_count_left++;
                if (press_count_left == 2) {
                    press_count_left = 0;
                    return BUTTON_DOUBLE_PRESS;
                }
            } else {
                press_count_left = 1;
            }
            last_press_time_left = current_time;
            return BUTTON_SINGLE_PRESS;
        }
    } else if (button_id == 1) {
        if (gpio_read_pin(GPIOA, 1)) {
            if (current_time - last_press_time_right < 200) {
                press_count_right++;
                if (press_count_right == 2) {
                    press_count_right = 0;
                    return BUTTON_DOUBLE_PRESS;
                }
            } else {
                press_count_right = 1;
            }
            last_press_time_right = current_time;
            return BUTTON_SINGLE_PRESS;
        }
    }
    return BUTTON_NO_PRESS;
}

// Definir un umbral de tiempo para la detección de pulsaciones largas
#define LONG_PRESS_THRESHOLD 1000  // 1000 ms (1 segundo)

static uint32_t button_press_time = 0;  // Variable para almacenar el tiempo de presión de los botones

// Función para detectar un evento de botón izquierdo (pulsación corta o larga)
int detect_button_event_left(void) {
    // Detectar la pulsación del botón izquierdo y medir el tiempo de presión
    if (gpio_button_is_pressed()){
        button_press_time++;
        if (button_press_time > LONG_PRESS_THRESHOLD) {
            // Se detecta una pulsación larga
            return 2;  // Valor para pulsación larga
        }
        return 1;  // Pulsación corta
    } else {
        button_press_time = 0;  // Resetear el tiempo de presión si el botón no está presionado
    }
    return 0;  // No hay evento
}

// Función para detectar un evento de botón derecho (pulsación corta o larga)
int detect_button_event_right(void) {
    // Detectar la pulsación del botón derecho y medir el tiempo de presión
    if (gpio_button_right_is_pressed()){
        button_press_time++;
        if (button_press_time > LONG_PRESS_THRESHOLD) {
            // Se detecta una pulsación larga
            return 2;  // Valor para pulsación larga
        }
        return 1;  // Pulsación corta
    } else {
        button_press_time = 0;  // Resetear el tiempo de presión si el botón no está presionado
    }
    return 0;  // No hay evento
}

// Función para detectar el comando de parada (pulsación larga o evento específico)
    // Detectar el botón de parada, por ejemplo, cuando se presiona una vez por un largo tiempo
int detect_stop_event(void) {
    // Detectar el botón de parada, por ejemplo, cuando se presiona una vez por un largo tiempo
    if (gpio_button_left_is_pressed() && gpio_button_right_is_pressed()) {
        // Se puede detectar un evento si ambos botones están presionados simultáneamente
        return 1;  // Comando de parada detectado
    }
    return 0;  // No se detecta el comando de parada
}
 
// Definimos un umbral de tiempo para detectar pulsaciones largas (en milisegundos)
#define LONG_PRESS_THRESHOLD 1000  // 1000 ms = 1 segundo

// Variable global para almacenar el tiempo de presión de los botones
static uint32_t button_press_time_left = 0;
static uint32_t button_press_time_right = 0;

// Función para detectar eventos de botones izquierdo y derecho
int detect_button_event(void) {
    int event = 0;

    // Detectar el evento del botón izquierdo
    if (gpio_button_left_is_pressed()) {
        button_press_time_left++;  // Incrementar el tiempo de presión del botón izquierdo

        if (button_press_time_left >= LONG_PRESS_THRESHOLD) {
            event = 2;  // Pulsación larga detectada en el botón izquierdo
        } else if (button_press_time_left > 0 && button_press_time_left < LONG_PRESS_THRESHOLD) {
            event = 1;  // Pulsación corta detectada en el botón izquierdo
        }
    } else {
        button_press_time_left = 0;  // Reiniciar el contador de tiempo cuando el botón izquierdo se suelta
    }

    // Detectar el evento del botón derecho
    if (gpio_button_right_is_pressed()) {
        button_press_time_right++;  // Incrementar el tiempo de presión del botón derecho

        if (button_press_time_right >= LONG_PRESS_THRESHOLD) {
            event = 4;  // Pulsación larga detectada en el botón derecho
        } else if (button_press_time_right > 0 && button_press_time_right < LONG_PRESS_THRESHOLD) {
            event = 3;  // Pulsación corta detectada en el botón derecho
        }
    } else {
        button_press_time_right = 0;  // Reiniciar el contador de tiempo cuando el botón derecho se suelta
    }

    // Si ambos botones están presionados, podríamos definir un evento especial (ej. comando de parada)
    if (gpio_button_left_is_pressed() && gpio_button_right_is_pressed()) {
        event = 5;  // Evento de ambos botones presionados simultáneamente
    }

    return event;  // Retorna el evento detectado
}

