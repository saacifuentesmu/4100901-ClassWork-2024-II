#include "led_driver.h"
#include "gpio.h"
#include "systick.h"

static uint8_t blink_counter_left = 0;
static uint8_t blink_counter_right = 0;
static uint8_t blink_active_left = 0;
static uint8_t blink_active_right = 0;

void led_on(uint8_t led_id) {
    if (led_id == LED_LEFT) {
        gpio_write_pin(GPIOA, 5, 1); // Enciende el LED izquierdo
    } else if (led_id == LED_RIGHT) {
        gpio_write_pin(GPIOA, 6, 1); // Enciende el LED derecho
    }
}

void led_off(uint8_t led_id) {
    if (led_id == LED_LEFT) {
        gpio_write_pin(GPIOA, 5, 0);
    } else if (led_id == LED_RIGHT) {
        gpio_write_pin(GPIOA, 6, 0);
    }
}

void led_toggle(uint8_t led_id) {
    if (led_id == LED_LEFT) {
        gpio_toggle_pin(GPIOA, 5);
    } else if (led_id == LED_RIGHT) {
        gpio_toggle_pin(GPIOA, 6);
    }
}

void led_blink(uint8_t led_id, uint16_t blink_times, uint16_t interval_ms) {
    if (led_id == LED_LEFT) {
        blink_active_left = 1;
        blink_counter_left = blink_times * 2; // Encendido + apagado
        systick_start_timer(interval_ms);
    } else if (led_id == LED_RIGHT) {
        blink_active_right = 1;
        blink_counter_right = blink_times * 2;
        systick_start_timer(interval_ms);
    }
}

void systick_callback() {
    if (blink_active_left && blink_counter_left > 0) {
        led_toggle(LED_LEFT);
        blink_counter_left--;
        if (blink_counter_left == 0) {
            blink_active_left = 0;
            led_off(LED_LEFT);
        }
    }
    if (blink_active_right && blink_counter_right > 0) {
        led_toggle(LED_RIGHT);
        blink_counter_right--;
        if (blink_counter_right == 0) {
            blink_active_right = 0;
            led_off(LED_RIGHT);
        }
    }
}
