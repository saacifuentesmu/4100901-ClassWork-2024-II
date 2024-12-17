
#include <stdint.h>
#include "gpio.h"
void configure_systick_and_start(void);
uint32_t systick_GetTick(void);
void systick_reset(void);
void systick_init(void);    // Inicializa el temporizador SysTick

uint32_t systick_get_ms(void);      // Obtiene el número de milisegundos desde la inicialización de SysTick
void SysTick_Handler(void);
void systick_delay_ms(uint32_t ms); // Retardo en milisegundos utilizando SysTick

uint32_t systick_get_ticks_gpio_read_pin(GPIO_t *GPIOx, uint8_t pin);
void systick_start_timer(uint32_t ticks);
