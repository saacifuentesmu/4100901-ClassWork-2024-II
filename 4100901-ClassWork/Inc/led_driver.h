#include "stdint.h"
#ifndef LED_DRIVER_H
#define LED_DRIVER_H

void led_on(uint8_t led_id);
void led_off(uint8_t led_id);
void led_toggle(uint8_t led_id);
void led_blink(uint8_t led_id, uint16_t blink_times, uint16_t interval_ms);

#endif // LED_DRIVER_H
