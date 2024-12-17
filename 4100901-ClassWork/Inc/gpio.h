#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

#define GPIOA ((GPIO_t *)0x48000000) // Dirección base de GPIOA
#define GPIO_INPUD 0x0 // Modo de entrada
// extern #define LED_PIN 5       // Pin 5 de GPIOA
// extern #define LED_LEFT 6      // Pin 14 de GPIOA (LED izquierdo)
// extern #define LED_RIGHT 7     // Pin 15 de GPIOA (LED derecho)
#define LED_PIN 5    // Pin 5 of GPIOA
#define LED_LEFT 6   // Pin 14 of GPIOA (Led left)
#define LED_RIGHT 7  // Pin 15 de GPIOA (Led right)

typedef struct {
    volatile uint32_t MEMRMP;
    volatile uint32_t CFGR1;
    volatile uint32_t EXTICR[4];
    volatile uint32_t CFGR2;
} SYSCFG_t;

typedef struct {
    volatile uint32_t IMR1;
    volatile uint32_t EMR1;
    volatile uint32_t RTSR1;
    volatile uint32_t FTSR1;
    volatile uint32_t SWIER1;
    volatile uint32_t PR1;
    volatile uint32_t IMR2;
    volatile uint32_t EMR2;
    volatile uint32_t RTSR2;
    volatile uint32_t FTSR2;
    volatile uint32_t SWIER2;
    volatile uint32_t PR2;
} EXTI_t;


typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
    volatile uint32_t BRR;

} GPIO_t;

void init_gpio_pin(GPIO_t *GPIOx, uint8_t pin, uint8_t mode);
void configure_gpio(void);

uint8_t gpio_button_is_pressed(void);
uint8_t gpio_button_left_is_pressed(void);
uint8_t gpio_button_right_is_pressed(void);
void gpio_toggle_led(void);
void gpio_toggle_led_left(void);
void gpio_toggle_led_right(void);
void gpio_off_led_right(void);
void gpio_off_led_left(void);
// Interrupt handler for EXTI lines
void EXTI15_10_IRQHandler(void);

// void uart_command_handler(void);
void gpio_turn_off_all_leds(void);

uint8_t gpio_read_pin(GPIO_t *GPIOx, uint8_t pin);
void gpio_write_pin(GPIO_t *GPIOx, uint8_t pin, uint8_t value);
void gpio_toggle_pin(GPIO_t *GPIOx, uint8_t pin);

#endif // GPIO_H
