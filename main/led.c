#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

void led_init(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    printf("LED initialized on GPIO %d\n", LED_PIN);
}

void led_set_state(int state)
{
    gpio_set_level(LED_PIN, state);
}

void led_blink_forever(void)
{
    printf("Starting LED blink...\n");

    while (1) {
        led_set_state(1);
        vTaskDelay(BLINK_DELAY_MS / portTICK_PERIOD_MS);

        led_set_state(0);
        vTaskDelay(BLINK_DELAY_MS / portTICK_PERIOD_MS);
    }
}
