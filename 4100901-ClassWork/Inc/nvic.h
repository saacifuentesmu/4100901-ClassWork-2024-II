#include <stdint.h>


typedef struct {
    volatile uint32_t ISER[8];  // Interrupt set-enable registers
    uint32_t RESERVED0[24];     // Reserved
    volatile uint32_t ICER[8];  // Interrupt clear-enable registers
    uint32_t RESERVED1[24];     // Reserved
    volatile uint32_t ISPR[8];  // Interrupt set-pending registers
    uint32_t RESERVED2[24];     // Reserved
    volatile uint32_t ICPR[8];  // Interrupt clear-pending registers
    uint32_t RESERVED3[24];     // Reserved
    volatile uint32_t IABR[8];  // Interrupt active bit registers
    uint32_t RESERVED4[56];     // Reserved
    volatile uint8_t  IP[240];  // Interrupt priority registers
    uint32_t RESERVED5[644];    // Reserved
    volatile uint32_t STIR;     // Software trigger interrupt register

} L476RG_NVIC_TypeDef;

#define NVIC ((L476RG_NVIC_TypeDef *) 0xE000E100UL)
