#include "uart.h"
#include "rcc.h"
#include "gpio.h"
#include "nvic.h"
#include "parking_lights.h"

void UART_clock_enable(USART_TypeDef * UARTx) {
    if (UARTx == USART1) {
        *RCC_APB2ENR |= RCC_APB2ENR_USART1EN;
    } else if (UARTx == USART2) {
        *RCC_APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    } else if (UARTx == USART3) {
        *RCC_APB1ENR1 |= RCC_APB1ENR1_USART3EN;
    }
}

void UART_Init (USART_TypeDef * UARTx) {
    UART_clock_enable(UARTx);
    // Disable USART
    UARTx->CR1 &= ~USART_CR1_UE;

    // Set data length to 8 bits (clear M bit)
    UARTx->CR1 &= ~USART_CR1_M;

    // Select 1 stop bit (clear STOP bits in CR2)
    UARTx->CR2 &= ~USART_CR2_STOP;

    // Set parity control as no parity (clear PCE bit)
    UARTx->CR1 &= ~USART_CR1_PCE;

    // Oversampling by 16 (clear OVER8 bit)
    UARTx->CR1 &= ~USART_CR1_OVER8;

    // Set Baud rate to 9600 using APB frequency (4 MHz)
    UARTx->BRR = BAUD_9600_4MHZ;

    // Enable transmission and reception
    UARTx->CR1 |= (USART_CR1_TE | USART_CR1_RE);

    // Enable USART
    UARTx->CR1 |= USART_CR1_UE;

    // Verify that USART is ready for transmission
    while ((UARTx->ISR & USART_ISR_TEACK) == 0);

    // Verify that USART is ready for reception
    while ((UARTx->ISR & USART_ISR_REACK) == 0);
}

void UART_send_char(USART_TypeDef * UARTx, char ch) { // Enviar un caracter
    // Wait until transmit data register is empty
    while ((UARTx->ISR & (1 << 7)) == 0); // Esperar hasta que TDR esté vacío

    // Send character
    UARTx->TDR = ch; // Escribir carácter en TDR
}

void UART_send_string(USART_TypeDef * UARTx, char * str) {  // Enviar una cadena
    // Send each character in the string
    while (*str) {
        UART_send_char(UARTx, *str++);  // Enviar cada carácter de la cadena
    }
}

uint8_t  UART_receive_char(USART_TypeDef * UARTx) {
    // Wait until data is received
    while ((UARTx->ISR & (1 << 5)) == 0);   // Esperar hasta que RDR contenga datos

    // Read received data
    return UARTx->RDR; // Leer dato recibido
}

void UART_receive_string(USART_TypeDef * UARTx, uint8_t *buffer, uint8_t len) {
    uint8_t i = 0;
    while (i < len) {
        buffer[i] = UART_receive_char(UARTx);
        i++;
    }
}

void UART_enable_nvic_it(USART_TypeDef * UARTx) {  // Habilitar interrupción en NVIC
    if (UARTx == USART1) {
        NVIC->ISER[1] |= (1 << 5); // Habilitar USART1
    } else if (UARTx == USART2) {
        NVIC->ISER[1] |= (1 << 6); // Habilitar USART2
    } else if (UARTx == USART3) {
        NVIC->ISER[1] |= (1 << 7); // Habilitar USART3
    }
}

uint8_t *rx_buffer;
uint8_t rx_len;
uint8_t rx_index;
uint8_t rx_ready;
void UART_receive_it(USART_TypeDef * UARTx, uint8_t *buffer, uint8_t len)
{
    UART_enable_nvic_it(UARTx); // Habilitar interrupción en NVIC
    // Enable receive interrupt
    UARTx->CR1 |= (1 << 5); // Habilitar interrupción por recepción (RXNEIE)
    // Set buffer and length
    rx_buffer = buffer; // Configurar buffer
    rx_len = len;       // Longitud de recepción
    rx_index = 0;       // Inicializar índice
}
void USART2_IRQHandler(void)
{
    // Check if the USART2 receive interrupt flag is set
    uint32_t isr = USART2->ISR;
    if (isr & (1 << 5)) {
        // Clear the interrupt flag
        USART2->ICR |= (1 << 5);
        // Read received data
        rx_buffer[rx_index] = USART2->RDR;
        rx_index++;
        if (rx_index == rx_len) {
            // Disable receive interrupt
            USART2->CR1 &= ~(1 << 5);
            rx_ready = 1;
        }
    }
}

uint8_t buffer[UART_BUFFER_]; // Definición del buffer y tamaño (10)

// Interrupción de UART para manejar la recepción de datos
void Uart(void) {
    if (rx_ready != 0) {
        UART_send_string(USART2, "Received: ");
        UART_send_string(USART2, (char *)buffer);
        UART_send_string(USART2, "\r\n");
        UART_receive_it(USART2, buffer, 1);
        rx_ready = 0;
        
        //UART STATES
        if (buffer[0] == 'S') {
            UART_send_string(USART2, "'Stationary'!\r\n");
            current_state = STATE_OFF; // Cambia al estado 2 si 'S' fue recibido
        }

        if (buffer[0] == 'L') {
            UART_send_string(USART2, "'Left'!\r\n");
            current_state = STATE_OFF; // Cambia al estado 6 si 'L' fue recibido
        }

        if (buffer[0] == 'l') {
            UART_send_string(USART2, "'Left infinite'!\r\n");
            current_state = STATE_OFF; // Cambia al estado 6 si 'L' fue recibido
        }

        if (buffer[0] == 'R') {
            UART_send_string(USART2, "'Right'!\r\n");
            current_state = STATE_OFF; // Cambia al estado 10 si 'R' fue recibido
        }

        if (buffer[0] == 'r') {
            UART_send_string(USART2, "'Right infinite'!\r\n");
            current_state = STATE_OFF; // Cambia al estado 10 si 'R' fue recibido
        }

        if (buffer[0] == 'O') {
            UART_send_string(USART2, "'IDLE'!\r\n");
            current_state = STATE_OFF; // Cambia al estado 0 si 'O' fue recibido
            gpio_off_led_left(); // Apaga LED 2
            gpio_off_led_right(); // Apaga LED 3
        }
    }
}
uint8_t rx__buffer[UART_BUFFER_SIZE]; // Definición del buffer y tamaño (1)

void uart_command_handler(void) {
    if (rx_ready) {
        char command = rx__buffer[0];  // Obtener el comando recibido

        if (command == 'l') {
            gpio_toggle_led_left();   // Parpadea LED izquierdo
        } else if (command == 'r') {
            gpio_toggle_led_right();  // Parpadea LED derecho
        } else if (command == 's') {
            gpio_toggle_led_left();   // Parpadea LED izquierdo
            gpio_toggle_led_right();  // Parpadea LED derecho
        } else if (command == 'o') {
            // Apaga ambos LEDs izquierdo y derecho
            GPIOA->ODR &= ~(1 << LED_LEFT);  
            GPIOA->ODR &= ~(1 << LED_RIGHT);
        }

        // Vuelve a habilitar la recepción de comandos por UART
        UART_receive_it(USART2, rx__buffer, UART_BUFFER_SIZE);
        rx_ready = 0;  // Resetear la variable de estado
    }
}