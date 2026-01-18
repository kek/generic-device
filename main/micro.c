#include "st7735.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#define ADC_CHANNEL ADC_CHANNEL_4  // GPIO 32 is ADC1_CH4
#define TAG "main"

void app_main(void)
{
    // Initialize ST7735 display
    st7735_init(INITR_BLACKTAB);

    // Set rotation to landscape (matching Arduino setRotation(1))
    st7735_set_rotation(1);

    // Fill screen with black
    st7735_fill_screen(ST77XX_BLACK);

    // Draw the letter "A" in white at position (10, 10) with size 2
    st7735_draw_char(10, 10, 'A', ST77XX_WHITE, ST77XX_BLACK, 2);

    // Configure ADC
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,  // 0-3.3V range
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &config);

    ESP_LOGI(TAG, "ADC1 Channel 4 (GPIO 32) configured");

    // Periodically read and log ADC value
    while (1) {
        int adc_raw;
        adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_raw);

        // Convert to voltage (assuming 12-bit ADC, 0-3.3V range)
        float voltage = (adc_raw / 4095.0) * 3.3;
        ESP_LOGI(TAG, "GPIO 32 ADC: %d (%.2fV)", adc_raw, voltage);

        // Display on screen
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.2fV", voltage);

        // Clear the text area and redraw
        st7735_fill_rect(10, 30, 100, 20, ST77XX_BLACK);
        st7735_draw_string(10, 30, buffer, ST77XX_WHITE, ST77XX_BLACK, 2);

        vTaskDelay(pdMS_TO_TICKS(60000));  // Delay 60 seconds to avoid loading the battery circuit
    }
}
