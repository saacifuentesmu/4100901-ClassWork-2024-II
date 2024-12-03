#include <stdint.h>

// GPIO structure definition
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

// GPIO base addresses
#define GPIOA ((GPIO_t *)0x48000000) // Base address of GPIOA
#define GPIOC ((GPIO_t *)0x48000800) // Base address of GPIOC

// GPIO utility macros
#define BUTTON_IS_PRESSED(GPIOx, pin)    (!(GPIOx->IDR & (1 << pin)))
#define BUTTON_IS_RELEASED(GPIOx, pin)   (GPIOx->IDR & (1 << pin))
#define TOGGLE_LED(GPIOx, pin)           (GPIOx->ODR ^= (1 << pin))

// Function prototypes
void init_gpio_pin(GPIO_t *GPIOx, uint8_t pin, uint8_t mode);
void enable_gpio_clock(uint32_t gpio_port);

