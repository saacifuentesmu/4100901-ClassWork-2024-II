#include "gpiox.h"

// Enable clock for GPIO port
void enable_gpio_clock(uint32_t gpio_port)
{
    volatile uint32_t *RCC_AHB2ENR = (uint32_t *)(0x4002104C); // RCC AHB2 peripheral clock enable register
    *RCC_AHB2ENR |= (1 << gpio_port);
}

// Initialize GPIO pin
void init_gpio_pin(GPIO_t *GPIOx, uint8_t pin, uint8_t mode)
{
    GPIOx->MODER &= ~(0x3 << (pin * 2)); // Clear MODER bits for this pin
    GPIOx->MODER |= (mode << (pin * 2)); // Set MODER bits for this pin
}
