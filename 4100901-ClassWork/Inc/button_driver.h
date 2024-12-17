#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include <stdint.h>

typedef enum {
    BUTTON_NO_PRESS,
    BUTTON_SINGLE_PRESS,
    BUTTON_DOUBLE_PRESS,
    BUTTON_LONG_PRESS
} ButtonEvent_t;

void button_init(void);
ButtonEvent_t button_read_event(uint8_t button_id);

// Función para detectar si el botón de hazard está presionado
// int gpio_button_is_pressed(void);

// Función para detectar un evento de botón izquierdo (por ejemplo, pulsación corta o larga)
int detect_button_event_left(void);

// Función para detectar un evento de botón derecho (por ejemplo, pulsación corta o larga)
int detect_button_event_right(void);

// Función para detectar el comando de parada (por ejemplo, una pulsación larga o un evento específico)
int detect_stop_event(void);
int detect_button_event(void);
#endif // BUTTON_DRIVER_H
