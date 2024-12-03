#include "gpiox.h"
#include "systick.h"

#define LED_PIN 5 // Pin 5 of GPIOA
#define BUTTON_PIN 13 // Pin 13 of GPIOC

#define SYSCFG_BASE 0x40010000
#define SYSCFG ((SYSCFG_t *)SYSCFG_BASE)

#define RCC_APB2ENR ((uint32_t *)(0x40021060)) // RCC APB2 peripheral clock enable register
#define EXTI ((EXTI_t *)0x40010400)

#define EXTI15_10_IRQn 40
#define NVIC_ISER1 ((uint32_t *)(0xE000E104)) // NVIC Interrupt Set-Enable Register

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

volatile uint8_t button_pressed = 0;

void configure_gpio(void)
{
    enable_gpio_clock(0); // Enable GPIOA clock
    enable_gpio_clock(2); // Enable GPIOC clock

    *RCC_APB2ENR |= (1 << 0); // Enable clock for SYSCFG

    // Configure SYSCFG EXTICR to map EXTI13 to PC13
    SYSCFG->EXTICR[3] &= ~(0xF << 4);
    SYSCFG->EXTICR[3] |= (0x2 << 4);

    // Configure EXTI13 for falling edge trigger
    EXTI->FTSR1 |= (1 << BUTTON_PIN);
    EXTI->RTSR1 &= ~(1 << BUTTON_PIN);

    // Unmask EXTI13
    EXTI->IMR1 |= (1 << BUTTON_PIN);

    init_gpio_pin(GPIOA, LED_PIN, 0x1); // Configure LED pin as output
    init_gpio_pin(GPIOC, BUTTON_PIN, 0x0); // Configure button pin as input

    // Enable EXTI15_10 interrupt
    *NVIC_ISER1 |= (1 << (EXTI15_10_IRQn - 32));
}

int main(void)
{
    configure_systick_and_start();
    configure_gpio();

    uint8_t state = 0;

    while (1) {
        switch (state) {
        case 0:
            if (button_pressed != 0) {
                state = 1;
            } else if (systick_GetTick() >= 500) {
                state = 2;
            }
            break;
        case 1:
            if (BUTTON_IS_RELEASED(GPIOC, BUTTON_PIN)) {
                button_pressed = 0;
                systick_reset();
                state = 0;
            }
            break;
        case 2:
            TOGGLE_LED(GPIOA, LED_PIN);
            systick_reset();
            state = 0;
            break;
        default:
            break;
        }
    }
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR1 & (1 << BUTTON_PIN)) {
        EXTI->PR1 = (1 << BUTTON_PIN);
        button_pressed = 1;
    }
}
