#ifndef LED_H
#define LED_H

#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_2
#define BLINK_DELAY_MS 1000

/**
 * Initialize the LED GPIO pin
 */
void led_init(void);

/**
 * Set LED state
 * @param state 1 for on, 0 for off
 */
void led_set_state(int state);

/**
 * Blink the LED continuously with configured delay
 * This function runs in an infinite loop
 */
void led_blink_forever(void);

#endif // LED_H
