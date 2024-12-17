#include "systick.h"
#include "gpio.h"

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;

} SysTick_t;


#define SysTick ((SysTick_t *)0xE000E010) // Base address of SysTick
#define SystemCoreClock 4000000 // Por ejemplo, si tu reloj del sistema es 4 MHz
#define SysTick_CTRL_CLKSOURCE_Msk   (1UL << 2)  // Selección de fuente de reloj (1 = reloj del procesador)
#define SysTick_CTRL_TICKINT_Msk     (1UL << 1)  // Habilita la interrupción de SysTick (1 = habilitada)
#define SysTick_CTRL_ENABLE_Msk      (1UL << 0)  // Habilita SysTick (1 = habilitado)
// void systick_init(void) {
//     // Configura el SysTick para generar interrupciones cada 1 ms
//     SysTick->LOAD = (SystemCoreClock / 1000) - 1; // Carga para 1 ms
//     SysTick->VAL = 0; // Reinicia el valor actual
//     SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
// }
void systick_start_timer(uint32_t ticks) {
    SysTick->LOAD = ticks - 1; // Carga el valor de los ticks
    SysTick->VAL = 0; // Resetea el contador de SysTick
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk;  // Habilita SysTick
}

void systick_init(void) {
    SysTick->CTRL = 0x4;     // Desactiva SysTick durante la configuración
    SysTick->LOAD = (SystemCoreClock / 1000) - 1; // Configura para 1 ms
    SysTick->VAL = 0;       // Reinicia el valor actual
    SysTick->CTRL = 0x7;    // Activa SysTick, usa el reloj del procesador
}


volatile uint32_t ms_counter = 0; // Counter for milliseconds


void configure_systick_and_start(void)
{
    SysTick->CTRL = 0x4;     // Disable SysTick for configuration, use processor clock
    SysTick->LOAD = 3999;    // Reload value for 1 ms (assuming 4 MHz clock)
    SysTick->CTRL = 0x7;     // Enable SysTick, processor clock, no interrupt
}

uint32_t systick_GetTick(void)
{
    return ms_counter;
}

void systick_reset(void)
{
    ms_counter = 0;
}

void SysTick_Handler(void)
{
    ms_counter++;
}
volatile uint32_t systick_counter = 0;
uint32_t systick_get_ms(void) {
    return systick_counter;
}
void systick_delay_ms(uint32_t ms) {
    uint32_t start = systick_counter; // Guarda el valor inicial del contador
    while ((systick_counter - start) < ms) {
        // Bucle activo esperando que pase el tiempo
    }
}

uint32_t systick_get_ticks_gpio_read_pin(GPIO_t *GPIOx, uint8_t pin) {
    uint32_t ticks = systick_GetTick();  // Obtiene el número actual de ticks
    uint32_t pin_state = (GPIOx->IDR & (1 << pin)) ? 1 : 0; // Lee el estado del pin
    return (ticks << 1) | pin_state; // Retorna los ticks y el estado del pin
}
