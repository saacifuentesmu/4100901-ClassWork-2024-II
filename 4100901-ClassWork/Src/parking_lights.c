#include "parking_lights.h"
#include "uart.h"
#include "systick.h"
#include "gpio.h"
#include "led_driver.h"
#include "button_driver.h"

// Variable estática que mantiene el estado actual de la máquina de estados
ParkingLightState_t current_state = STATE_OFF;
uint32_t heartbeat;

uint8_t i = 0; // count number
void fsm_handle_events(void){//int hazard_button, int left_evt, int right_evt, int stop_command){
    Uart();
    // Inicialización de heartbeat si es necesario
    if (heartbeat == 0) {
        heartbeat = systick_GetTick();  // Asigna valor inicial
    }

    switch (current_state) {
        case STATE_OFF:
            if (gpio_button_is_pressed() != 0) {
                current_state = STATE_HAZARD;
                heartbeat = systick_GetTick();
            } else if (gpio_button_left_is_pressed() != 0) {   // Button left
                if (gpio_button_left_is_pressed() == 2 ) {    // Doble pulsacion
                    current_state = STATE_TURN_LEFT_INFINITE;
                }else {
                    current_state = STATE_TURN_LEFT_3BLINKS;
                }
                heartbeat = systick_GetTick();
            } else if (gpio_button_right_is_pressed() != 0) {   // Button right
                if (gpio_button_right_is_pressed() == 2 ) {    // Doble pulsacion
                    current_state = STATE_TURN_RIGHT_INFINITE;
                } else {
                    current_state = STATE_TURN_RIGHT_3BLINKS;
                }
                heartbeat = systick_GetTick();
            }
            break;

        case STATE_HAZARD:
            if (systick_GetTick() >= heartbeat + 500) { 
                UART_send_string(USART2, "STATE HAZARD\r\n"); 
                gpio_toggle_led();
                gpio_toggle_led_left();
                gpio_toggle_led_right();
                heartbeat = systick_GetTick();
            } 
            if (gpio_button_is_pressed() != 0) {    // Botón central para detener HAZARD
                gpio_off_led_left();
                gpio_off_led_right();
                current_state = STATE_OFF;  // Regresar a OFF después de 3 parpadeos
            }
            break;

        case STATE_TURN_LEFT_3BLINKS:
            if (i < 6 && systick_GetTick() >= heartbeat + 500) {
                UART_send_string(USART2, "STATE_TURN_LEFT_3BLINKS\r\n");
                gpio_toggle_led_left();
                heartbeat = systick_GetTick();
                i++;
            } else if (i >= 6) {
                gpio_off_led_left();
                i = 0;
                current_state = STATE_OFF;
            }
            break;

        case STATE_TURN_LEFT_INFINITE:
            if (systick_GetTick() >= heartbeat + 500) {
                UART_send_string(USART2, "STATE_TURN_LEFT_INFINITE\r\n");
                gpio_toggle_led_left(); // Toggle LED izquierdo
                heartbeat = systick_GetTick();
            }
            if (gpio_button_right_is_pressed() != 0 || gpio_button_is_pressed() != 0) { 
                gpio_off_led_left(); // Botón derecho o central detiene el parpadeo
                current_state = STATE_OFF;
            }
            break;

        case STATE_TURN_RIGHT_3BLINKS:
            if (i < 6 && systick_GetTick() >= heartbeat + 500) {
                UART_send_string(USART2, "STATE_TURN_RIGHT_3BLINKS\r\n"); 
                gpio_toggle_led_right(); // Toggle LED derecho
                heartbeat = systick_GetTick();
                i++;
            } else if (i >= 6) {
                gpio_off_led_right();
                i = 0;
                current_state = STATE_OFF; // Regresa a OFF
            }
            break;
            
        case STATE_TURN_RIGHT_INFINITE:
            if (systick_GetTick() >= heartbeat + 500) {
                UART_send_string(USART2, "STATE_TURN_RIGHT_INFINITE\r\n"); 
                gpio_toggle_led_right(); // Toggle LED derecho
                heartbeat = systick_GetTick();
            }
            if (gpio_button_right_is_pressed() != 0 || gpio_button_is_pressed() != 0) { 
                gpio_off_led_right(); // Botón izquierdo o central detiene el parpadeo
                current_state = STATE_OFF;
            }
            break;
    }
}
