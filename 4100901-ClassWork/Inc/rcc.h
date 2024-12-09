
#define RCC_BASE 0x40021000

#define RCC_AHB2ENR ((uint32_t *)(RCC_BASE + 0x4C))
#define RCC_APB1ENR1 ((uint32_t *)(RCC_BASE + 0x58)) // APB1 peripheral clock enable register 1
#define RCC_APB2ENR ((uint32_t *)(RCC_BASE + 0x60)) // APB2 peripheral clock enable register

#define RCC_APB2ENR_USART1EN (1 << 14) // USART1 clock enable
#define RCC_APB1ENR1_USART2EN (1 << 17) // USART2 clock enable
#define RCC_APB1ENR1_USART3EN (1 << 18) // USART3 clock enable
