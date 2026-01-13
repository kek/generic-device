#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_mac.h"

#define LED_PIN GPIO_NUM_2
void print_info();

void app_main(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    printf("Starting LED blink...\n");

    print_info();

    while (1) {
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        gpio_set_level(LED_PIN, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void print_info() {
    // Get and display chip information
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    printf("\n========== ESP32 Board Info ==========\n");
    printf("Chip model: ");
    switch(chip_info.model) {
        case CHIP_ESP32:   printf("ESP32\n"); break;
        case CHIP_ESP32S2: printf("ESP32-S2\n"); break;
        case CHIP_ESP32S3: printf("ESP32-S3\n"); break;
        case CHIP_ESP32C3: printf("ESP32-C3\n"); break;
        case CHIP_ESP32C2: printf("ESP32-C2\n"); break;
        case CHIP_ESP32C6: printf("ESP32-C6\n"); break;
        case CHIP_ESP32H2: printf("ESP32-H2\n"); break;
        default: printf("Unknown (0x%x)\n", chip_info.model); break;
    }

    printf("Chip revision: %d\n", chip_info.revision);
    printf("CPU cores: %d\n", chip_info.cores);

    printf("Features: ");
    if (chip_info.features & CHIP_FEATURE_WIFI_BGN) printf("WiFi ");
    if (chip_info.features & CHIP_FEATURE_BLE) printf("BLE ");
    if (chip_info.features & CHIP_FEATURE_BT) printf("BT ");
    if (chip_info.features & CHIP_FEATURE_EMB_FLASH) printf("Flash ");
    if (chip_info.features & CHIP_FEATURE_EMB_PSRAM) printf("PSRAM ");
    if (chip_info.features & CHIP_FEATURE_IEEE802154) printf("IEEE802154 ");
    printf("\n");

    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    printf("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    printf("======================================\n\n");
}
