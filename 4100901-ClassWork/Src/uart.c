#include "uart.h"
#include "rcc.h"

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

    // Set Baud rate to 9600 using APB frequency (80 MHz)
    UARTx->BRR = BAUD_9600_80MHZ;

    // Enable transmission and reception
    UARTx->CR1 |= (USART_CR1_TE | USART_CR1_RE);

    // Enable USART
    UARTx->CR1 |= USART_CR1_UE;

    // Verify that USART is ready for transmission
    while ((UARTx->ISR & USART_ISR_TEACK) == 0);

    // Verify that USART is ready for reception
    while ((UARTx->ISR & USART_ISR_REACK) == 0);
}

void UART_send_char(USART_TypeDef * UARTx, char ch) {
    // Wait until transmit data register is empty
    while ((UARTx->ISR & (1 << 7)) == 0);

    // Send character
    UARTx->TDR = ch;
}

void UART_send_string(USART_TypeDef * UARTx, char * str) {
    // Send each character in the string
    while (*str) {
        UART_send_char(UARTx, *str++);
    }
}

