#include <stdint.h>
#include "systick.h"
#include "gpio.h"
#include "uart.h"
#include "parking_lights.h"

int main(void){
    // systick_init(); // Inicializa el temporizador SysTick
     
    configure_gpio();   // Configura los pines GPIO
    
    UART_Init(USART2);  // Inicializa la comunicación UART

    //uint8_t state = 0; // state of the FSM
    
    UART_send_string(USART2, "Hello World, from main!\r\n");    // Enviar mensaje por UART
    // gpio_toggle_led();
    // uint8_t the_buffer[10]; // Buffer para recibir datos
    // UART_receive_string(USART2, the_buffer, 10);    // Recibir datos de UART
// Enviar datos recibidos de nuevo por UART
    // UART_send_string(USART2, "Received: ");
    // UART_send_string(USART2, (char *)the_buffer);
    // UART_send_string(USART2, "\r\n");

    // UART_receive_it(USART2, the_buffer, 10);    // Inicia recepción en modo interrupción
    // Variables para los eventos de los botones
    uint8_t expression = 0;
    while (1) {
        switch (expression)
        {
        case 0:
            if (gpio_button_is_pressed() != 0 ){
                UART_send_string(USART2, "precionado\r\n");    // Enviar mensaje por UART
                
                expression = 1;
            }
                break;
        case 1:
            gpio_toggle_led();
            UART_send_string(USART2, "LLego mensaje\r\n");  // Enviar mensaje por UART
            expression = 0;
            break;
        default:
            break;
        }
    }
}
