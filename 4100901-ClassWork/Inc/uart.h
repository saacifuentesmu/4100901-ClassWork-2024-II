#include <stdint.h>

// Base addresses for USART peripherals (assuming STM32L4 family)
#define USART1_BASE  0x40013800UL
#define USART2_BASE  0x40004400UL
#define USART3_BASE  0x40004800UL

// Define USART_TypeDef structure to match hardware registers
typedef struct {
    volatile uint32_t CR1;    // Control register 1
    volatile uint32_t CR2;    // Control register 2
    volatile uint32_t CR3;    // Control register 3
    volatile uint32_t BRR;    // Baud rate register
    volatile uint32_t GTPR;   // Guard time and prescaler register
    volatile uint32_t RTOR;   // Receiver timeout register
    volatile uint32_t RQR;    // Request register
    volatile uint32_t ISR;    // Interrupt and status register
    volatile uint32_t ICR;    // Interrupt flag clear register
    volatile uint32_t RDR;    // Receive data register
    volatile uint32_t TDR;    // Transmit data register
} USART_TypeDef;

// Map USART base addresses to USART_TypeDef structures
#define USART1 ((USART_TypeDef *) USART1_BASE)
#define USART2 ((USART_TypeDef *) USART2_BASE)
#define USART3 ((USART_TypeDef *) USART3_BASE)

// Bit definitions for USART_CR1 register
#define USART_CR1_UE      (1U << 0)    // USART enable
#define USART_CR1_M       (1U << 12)   // Word length (0 = 8 bits, 1 = 9 bits)
#define USART_CR1_PCE     (1U << 10)   // Parity control enable
#define USART_CR1_OVER8   (1U << 15)   // Oversampling mode
#define USART_CR1_TE      (1U << 3)    // Transmitter enable
#define USART_CR1_RE      (1U << 2)    // Receiver enable

// Bit definitions for USART_CR2 register
#define USART_CR2_STOP    (3U << 12)   // STOP bits (00 = 1 Stop bit)

// Bit definitions for USART_ISR register
#define USART_ISR_TEACK   (1U << 21)   // Transmit enable acknowledge flag
#define USART_ISR_REACK   (1U << 22)   // Receive enable acknowledge flag

// Baud rate register definition for 4 MHz APB clock and 9600 baud rate
#define BAUD_9600_4MHZ   (0x1A1)       // Calculated value for 9600 baud rate with 4 MHz APB clock


void UART_Init (USART_TypeDef * UARTx);
void UART_send_char(USART_TypeDef * UARTx, char ch);
void UART_send_string(USART_TypeDef * UARTx, char * str);

void UART_receive_string(USART_TypeDef * UARTx, uint8_t *buffer, uint8_t len);
