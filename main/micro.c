#include "st7735.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
        ESP_LOGI(TAG, "GPIO 32 ADC value: %d", adc_raw);

        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay 1 second
    }
}
