#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

/**
 * Print comprehensive ESP32 chip information including:
 * - Chip model
 * - Chip revision
 * - CPU cores
 * - Available features (WiFi, BLE, BT, Flash, PSRAM, etc.)
 * - MAC address
 */
void system_info_print(void);

#endif // SYSTEM_INFO_H
