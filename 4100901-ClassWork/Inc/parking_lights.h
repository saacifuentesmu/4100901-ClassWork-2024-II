#include <stdint.h>
#include "button_driver.h"
#ifndef PARKING_LIGHTS_H
#define PARKING_LIGHTS_H

typedef enum {
    STATE_OFF,
    STATE_HAZARD,
    STATE_TURN_LEFT_3BLINKS,
    STATE_TURN_LEFT_INFINITE,
    STATE_TURN_RIGHT_3BLINKS,
    STATE_TURN_RIGHT_INFINITE
} ParkingLightState_t;

// Declaración de la variable global sin 'static'
extern ParkingLightState_t current_state; // Declarar la variable como 'extern' para que sea accesible en otros archivos

void fsm_handle_events(void);//int hazard_button, int left_evt, int right_evt, int stop_command);

#endif // PARKING_LIGHTS_H
