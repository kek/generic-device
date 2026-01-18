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
    st7735_init(INITR_BLACKTAB);

    st7735_set_rotation(1);

    st7735_fill_screen(ST77XX_BLACK);

    st7735_draw_string(10, 10, "Text goes here", ST77XX_WHITE, ST77XX_BLACK, 1);

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,  // 0-3.3V range
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &config);

    ESP_LOGI(TAG, "ADC1 Channel 4 (GPIO 32) configured");

    // Periodically read and log ADC value
    while (1) {
        // Wait 5 minutes to let the circuit fully recover
        vTaskDelay(pdMS_TO_TICKS(300000));  // 5 minutes

        // Take just 3 samples with long delays
        int adc_raw = 0;
        for (int i = 0; i < 3; i++) {
            adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_raw);
            ESP_LOGI(TAG, "  Sample %d: %d", i+1, adc_raw);
            if (i < 2) {
                vTaskDelay(pdMS_TO_TICKS(2000));  // 2 seconds between samples
            }
        }

        // Convert to voltage (assuming 12-bit ADC, 0-3.3V range)
        float adc_voltage = (adc_raw / 4095.0) * 3.3;

        // Apply voltage divider multiplier (adjust based on actual measurement)
        double voltage_measurement_multiplier = 3.43;
        float battery_voltage =
            adc_voltage * voltage_measurement_multiplier; // Adjust this multiplier!

        ESP_LOGI(TAG, "Battery reading - ADC: %d (ADC: %.2fV, Battery: %.2fV)", adc_raw, adc_voltage, battery_voltage);

        // Display on screen
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.2fV", battery_voltage);

        // Clear the text area and redraw
        st7735_fill_rect(10, 30, 100, 20, ST77XX_BLACK);
        st7735_draw_string(10, 30, buffer, ST77XX_WHITE, ST77XX_BLACK, 1);
    }
}
