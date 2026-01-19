#ifndef ST7735_H
#define ST7735_H

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"

// Pin definitions (HSPI defaults)
#define TFT_CS      GPIO_NUM_15
#define TFT_RST     GPIO_NUM_4
#define TFT_DC      GPIO_NUM_18
#define TFT_MOSI    GPIO_NUM_13
#define TFT_SCLK    GPIO_NUM_14
#define TFT_BL      GPIO_NUM_27

// Display dimensions
#define ST7735_WIDTH    128
#define ST7735_HEIGHT   160

// Color definitions (RGB565)
#define ST77XX_BLACK    0x0000
#define ST77XX_WHITE    0xFFFF
#define ST77XX_RED      0xF800
#define ST77XX_GREEN    0x07E0
#define ST77XX_BLUE     0x001F
#define ST77XX_CYAN     0x07FF
#define ST77XX_MAGENTA  0xF81F
#define ST77XX_YELLOW   0xFFE0
#define ST77XX_ORANGE   0xFC00

// Rotation values
#define ST7735_ROTATION_0   0
#define ST7735_ROTATION_90  1
#define ST7735_ROTATION_180 2
#define ST7735_ROTATION_270 3

// Init types
#define INITR_BLACKTAB  0
#define INITR_GREENTAB  1

/**
 * Initialize the ST7735 display with software SPI
 * @param init_type INITR_BLACKTAB or INITR_GREENTAB
 * @return ESP_OK on success
 */
esp_err_t st7735_init(uint8_t init_type);

/**
 * Set display rotation
 * @param rotation 0-3 for different orientations
 */
void st7735_set_rotation(uint8_t rotation);

/**
 * Fill entire screen with a color
 * @param color RGB565 color value
 */
void st7735_fill_screen(uint16_t color);

/**
 * Draw a rectangle outline
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h Height
 * @param color RGB565 color value
 */
void st7735_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

/**
 * Draw a filled rectangle
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h Height
 * @param color RGB565 color value
 */
void st7735_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

/**
 * Draw a single character
 * @param x X coordinate
 * @param y Y coordinate
 * @param c Character to draw
 * @param color Text color (RGB565)
 * @param bg Background color (RGB565)
 * @param size Text size multiplier (1-8)
 */
void st7735_draw_char(int16_t x, int16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);

/**
 * Draw a string
 * @param x X coordinate
 * @param y Y coordinate
 * @param str String to draw
 * @param color Text color (RGB565)
 * @param bg Background color (RGB565)
 * @param size Text size multiplier (1-8)
 */
void st7735_draw_string(int16_t x, int16_t y, const char* str, uint16_t color, uint16_t bg, uint8_t size);

/**
 * Get current display width (respects rotation)
 */
int16_t st7735_width(void);

/**
 * Get current display height (respects rotation)
 */
int16_t st7735_height(void);

/**
 * Draw raw RGB565 image data
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width of image
 * @param h Height of image
 * @param data RGB565 image data (big-endian, 2 bytes per pixel)
 */
void st7735_draw_image(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *data);

#endif // ST7735_H
