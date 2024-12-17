#include "gpio.h"
#include "rcc.h"
#include "uart.h"
#include "systick.h"
#include "parking_lights.h"
#include "button_driver.h"
#include "led_driver.h"

#define EXTI_BASE 0x40010400
#define EXTI ((EXTI_t *)EXTI_BASE)

#define EXTI15_10_IRQn 40
#define NVIC_ISER1 ((uint32_t *)(0xE000E104)) // NVIC Interrupt Set-Enable Register

#define SYSCFG_BASE 0x40010000
#define SYSCFG ((SYSCFG_t *)SYSCFG_BASE)

#define GPIOA ((GPIO_t *)0x48000000) // Base address of GPIOA
#define GPIOC ((GPIO_t *)0x48000800) // Base address of GPIOC

#define LED_PIN 5    // Pin 5 of GPIOA
#define LED_LEFT 14   // Pin 14 of GPIOA (Led left) 6
#define LED_RIGHT 15  // Pin 15 de GPIOA (Led right) 7

#define BUTTON_PIN 13    // Pin 13 of GPIOC  1
#define BUTTON_LEFT 14   // Pin 14 de GPIOC (Button left) 4
#define BUTTON_RIGHT 15  // Pin 15 de GPIOC (Button right) 0

#define BUTTON_IS_PRESSED()    (!(GPIOC->IDR & (1 << BUTTON_PIN)))
#define BUTTON_LEFT_IS_PRESSED()   (!(GPIOC->IDR & (1 << BUTTON_LEFT)))
#define BUTTON_RIGHT_IS_PRESSED()  (!(GPIOC->IDR & (1 << BUTTON_RIGHT)))
#define BUTTON_IS_RELEASED()   (GPIOC->IDR & (1 << BUTTON_PIN))
#define BUTTON_LEFT_IS_RELEASED()  (GPIOC->IDR & (1 << BUTTON_LEFT))
#define BUTTON_RIGHT_IS_RELEASED()  (GPIOC->IDR & (1 << BUTTON_RIGHT))
#define TOGGLE_LED()           (GPIOA->ODR ^= (1 << LED_PIN))
#define TOGGLE_LED_LEFT()      (GPIOA->ODR ^= (1 << LED_LEFT))  // LED_LEFT
#define TOGGLE_LED_RIGHT()     (GPIOA->ODR ^= (1 << LED_RIGHT))  // LED_RIGHT

#define OFF_LED_LEFT() (GPIOA->ODR &= ~(1 << LED_LEFT))
#define OFF_LED_RIGHT() (GPIOA->ODR &= ~(1 << LED_RIGHT))

#define GPIO_INPUD 0x0 // Modo de entrada

volatile uint8_t button_pressed = 0; // Flag to indicate button press
volatile uint8_t button_pressed_left = 0;   // Flag to indicate button press
volatile uint8_t button_pressed_right = 0;  // Flag to indicate button press
void configure_gpio_for_usart() {
    // Enable GPIOA clock
    *RCC_AHB2ENR |= (1 << 0) | (1 << 2);    // Habilita GPIOA Y GPIOC

    // Configure PA2 (TX) as alternate function
    GPIOA->MODER &= ~(3U << (2 * 2)); // Clear mode bits for PA2
    GPIOA->MODER |= (2U << (2 * 2));  // Set alternate function mode for PA2

    // Configure PA3 (RX) as alternate function
    GPIOA->MODER &= ~(3U << (3 * 2)); // Clear mode bits for PA3
    GPIOA->MODER |= (2U << (3 * 2));  // Set alternate function mode for PA3

    // Set alternate function to AF7 for PA2 and PA3
    GPIOA->AFR[0] &= ~(0xF << (4 * 2)); // Clear AFR bits for PA2
    GPIOA->AFR[0] |= (7U << (4 * 2));   // Set AFR to AF7 for PA2
    GPIOA->AFR[0] &= ~(0xF << (4 * 3)); // Clear AFR bits for PA3
    GPIOA->AFR[0] |= (7U << (4 * 3));   // Set AFR to AF7 for PA3

    // Configure PA2 and PA3 as very high speed
    GPIOA->OSPEEDR |= (3U << (2 * 2)); // Very high speed for PA2
    GPIOA->OSPEEDR |= (3U << (3 * 2)); // Very high speed for PA3

    // Configure PA2 and PA3 as no pull-up, no pull-down
    GPIOA->PUPDR &= ~(3U << (2 * 2)); // No pull-up, no pull-down for PA2
    GPIOA->PUPDR &= ~(3U << (3 * 2)); // No pull-up, no pull-down for PA3
}

void init_gpio_pin(GPIO_t *GPIOx, uint8_t pin, uint8_t mode) {  // CONFIGURA GPIOx
    GPIOx->MODER &= ~(0x3 << (pin * 2)); // Clear MODER bits for this pin
    GPIOx->MODER |= (mode << (pin * 2)); // Set MODER bits for this pin
}

void configure_gpio(void) {
    *RCC_AHB2ENR |= (1 << 0) | (1 << 2); // Enable clock for GPIOA and GPIOC

    // Enable clock for SYSCFG
    *RCC_APB2ENR |= (1 << 0); // RCC_APB2ENR_SYSCFGEN

    // Configure SYSCFG EXTICR to map EXTI13, EXTI14, and EXTI15 to GPIOC
    SYSCFG->EXTICR[3] &= ~(0xF << 4); // Clear bits for EXTI13
    SYSCFG->EXTICR[3] |= (0x2 << 4);  // Map EXTI13 to Port C
    SYSCFG->EXTICR[3] &= ~(0xF << 8); // Clear bits for EXTI14
    SYSCFG->EXTICR[3] |= (0x2 << 8);  // Map EXTI14 to Port C
    SYSCFG->EXTICR[3] &= ~(0xF << 12); // Clear bits for EXTI15
    SYSCFG->EXTICR[3] |= (0x2 << 12);  // Map EXTI15 to Port C

    // Configure EXTI13, EXTI14, and EXTI15 for falling edge trigger
    EXTI->FTSR1 |= (1 << BUTTON_PIN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT);  // Enable falling trigger for all buttons
    EXTI->RTSR1 &= ~(1 << BUTTON_PIN) & ~(1 << BUTTON_LEFT) & ~(1 << BUTTON_RIGHT); // Disable rising trigger for all buttons

    // Unmask EXTI13, EXTI14, and EXTI15
    EXTI->IMR1 |= (1 << BUTTON_PIN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT);

    init_gpio_pin(GPIOA, LED_PIN, 0x1);   // Set LED pin as output
    init_gpio_pin(GPIOA, LED_LEFT, 0x1);  // Set LED_LEFT pin as output
    init_gpio_pin(GPIOA, LED_RIGHT, 0x1); // Set LED_RIGHT pin as output

    init_gpio_pin(GPIOC, BUTTON_PIN, 0x0);    // Set BUTTON pin as input
    init_gpio_pin(GPIOC, BUTTON_LEFT, 0x0);   // Set BUTTON_LEFT pin as input
    init_gpio_pin(GPIOC, BUTTON_RIGHT, 0x0);  // Set BUTTON_RIGHT pin as input

    // Enable EXTI15_10 interrupt
    *NVIC_ISER1 |= (1 << (EXTI15_10_IRQn - 32));

    configure_gpio_for_usart();
}

uint8_t gpio_button_is_pressed(void) {
    return BUTTON_IS_PRESSED();
}

uint8_t gpio_button_left_is_pressed(void) {
    return BUTTON_LEFT_IS_PRESSED();
}

uint8_t gpio_button_right_is_pressed(void) {
    return BUTTON_RIGHT_IS_PRESSED();
}

void gpio_toggle_led(void) {
    TOGGLE_LED();
}

void gpio_toggle_led_left(void) {
    TOGGLE_LED_LEFT();
}

void gpio_toggle_led_right(void) {
    TOGGLE_LED_RIGHT();
}
void gpio_off_led_right(void) {
    OFF_LED_RIGHT();
}
void gpio_off_led_left(void) {
    OFF_LED_LEFT();
}
void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR1 & (1 << BUTTON_PIN)) {
        EXTI->PR1 = (1 << BUTTON_PIN); // Clear pending bit
        button_pressed = 1; // Set button pressed flag
        //gpio_toggle_led(); // Toggle LED when BUTTON_PIN is pressed
    }
    if (EXTI->PR1 & (1 << BUTTON_LEFT)) {
        EXTI->PR1 = (1 << BUTTON_LEFT); // Clear pending bit
        button_pressed_left = 1; // Set button pressed flag
        // gpio_toggle_led_left(); // Toggle LED_LEFT when BUTTON_LEFT is pressed
    }
    if (EXTI->PR1 & (1 << BUTTON_RIGHT)) {
        EXTI->PR1 = (1 << BUTTON_RIGHT); // Clear pending bit
        button_pressed_right = 1; // Set button pressed flag
        // gpio_toggle_led_right(); // Toggle LED_RIGHT when BUTTON_RIGHT is pressed
    }
}

void gpio_turn_off_all_leds(void) {
    GPIOA->ODR &= ~(1 << LED_PIN);   // Apaga LED_PIN
    GPIOA->ODR &= ~(1 << LED_LEFT);  // Apaga LED_LEFT
    GPIOA->ODR &= ~(1 << LED_RIGHT); // Apaga LED_RIGHT
}

// Función para manejar los comandos recibidos por UART


uint8_t gpio_read_pin(GPIO_t *GPIOx, uint8_t pin) {
    return (GPIOx->IDR & (1 << pin)) ? 1 : 0;
}

void gpio_write_pin(GPIO_t *GPIOx, uint8_t pin, uint8_t value) {
    if (value == 1) {
        GPIOx->ODR |= (1 << pin);  // Set the bit to 1 (High)
    } else {
        GPIOx->ODR &= ~(1 << pin); // Set the bit to 0 (Low)
    }
}
void gpio_toggle_pin(GPIO_t *GPIOx, uint8_t pin) {
    GPIOx->ODR ^= (1 << pin);  // Toggle the bit using XOR
}
