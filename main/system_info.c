#include "system_info.h"
#include "esp_chip_info.h"
#include "esp_mac.h"
#include <stdio.h>
#include <string.h>

static const char* get_chip_model_name(esp_chip_model_t model)
{
    switch(model) {
        case CHIP_ESP32:   return "ESP32";
        case CHIP_ESP32S2: return "ESP32-S2";
        case CHIP_ESP32S3: return "ESP32-S3";
        case CHIP_ESP32C3: return "ESP32-C3";
        case CHIP_ESP32C2: return "ESP32-C2";
        case CHIP_ESP32C6: return "ESP32-C6";
        case CHIP_ESP32H2: return "ESP32-H2";
        default: return "Unknown";
    }
}

static void build_features_string(uint32_t features, char* buffer, size_t buffer_size)
{
    buffer[0] = '\0';

    if (features & CHIP_FEATURE_WIFI_BGN) {
        strncat(buffer, "WiFi ", buffer_size - strlen(buffer) - 1);
    }
    if (features & CHIP_FEATURE_BLE) {
        strncat(buffer, "BLE ", buffer_size - strlen(buffer) - 1);
    }
    if (features & CHIP_FEATURE_BT) {
        strncat(buffer, "BT ", buffer_size - strlen(buffer) - 1);
    }
    if (features & CHIP_FEATURE_EMB_FLASH) {
        strncat(buffer, "Flash ", buffer_size - strlen(buffer) - 1);
    }
    if (features & CHIP_FEATURE_EMB_PSRAM) {
        strncat(buffer, "PSRAM ", buffer_size - strlen(buffer) - 1);
    }
    if (features & CHIP_FEATURE_IEEE802154) {
        strncat(buffer, "IEEE802154 ", buffer_size - strlen(buffer) - 1);
    }

    // Remove trailing space if any
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == ' ') {
        buffer[len - 1] = '\0';
    }
}

void system_info_print(void)
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    printf("\n========== ESP32 Board Info ==========\n");

    const char* model_name = get_chip_model_name(chip_info.model);
    if (chip_info.model >= CHIP_ESP32 && chip_info.model <= CHIP_ESP32H2) {
        printf("Chip model: %s\n", model_name);
    } else {
        printf("Chip model: Unknown (0x%x)\n", chip_info.model);
    }

    printf("Chip revision: %d\n", chip_info.revision);
    printf("CPU cores: %d\n", chip_info.cores);

    char features[128];
    build_features_string(chip_info.features, features, sizeof(features));
    printf("Features: %s\n", features);

    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    printf("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    printf("======================================\n\n");
}
