#include "st7735.h"
#include "font5x7.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "ST7735";

// ST7735 Commands
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_SLPOUT  0x11
#define ST7735_NORON   0x13
#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_MADCTL  0x36
#define ST7735_COLMOD  0x3A
#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// MADCTL bits
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

static spi_device_handle_t spi;
static uint8_t _rotation = 0;
static int16_t _width = ST7735_WIDTH;
static int16_t _height = ST7735_HEIGHT;
static int16_t _xstart = 0;
static int16_t _ystart = 0;

// Helper functions
static void gpio_set_dc(uint8_t level) {
    gpio_set_level(TFT_DC, level);
}

static void gpio_set_rst(uint8_t level) {
    gpio_set_level(TFT_RST, level);
}

static void spi_write_byte(uint8_t data) {
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &data,
    };
    spi_device_polling_transmit(spi, &t);
}

static void spi_write_data(const uint8_t *data, size_t len) {
    if (len == 0) return;
    spi_transaction_t t = {
        .length = len * 8,
        .tx_buffer = data,
    };
    spi_device_polling_transmit(spi, &t);
}

static void write_command(uint8_t cmd) {
    gpio_set_dc(0);
    spi_write_byte(cmd);
}

static void write_data_byte(uint8_t data) {
    gpio_set_dc(1);
    spi_write_byte(data);
}

static void write_data(const uint8_t *data, size_t len) {
    gpio_set_dc(1);
    spi_write_data(data, len);
}

static void delay_ms(uint32_t ms) {
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

static void set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    x0 += _xstart;
    x1 += _xstart;
    y0 += _ystart;
    y1 += _ystart;

    write_command(ST7735_CASET);
    uint8_t data[] = {
        (uint8_t)(x0 >> 8),
        (uint8_t)(x0 & 0xFF),
        (uint8_t)(x1 >> 8),
        (uint8_t)(x1 & 0xFF)
    };
    write_data(data, 4);

    write_command(ST7735_RASET);
    data[0] = (uint8_t)(y0 >> 8);
    data[1] = (uint8_t)(y0 & 0xFF);
    data[2] = (uint8_t)(y1 >> 8);
    data[3] = (uint8_t)(y1 & 0xFF);
    write_data(data, 4);

    write_command(ST7735_RAMWR);
}

static void common_init(const uint8_t *cmdList) {
    uint8_t numCommands, cmd, numArgs;
    uint16_t ms;

    numCommands = *cmdList++;
    while (numCommands--) {
        cmd = *cmdList++;
        numArgs = *cmdList++;
        ms = numArgs & 0x80;
        numArgs &= ~0x80;

        write_command(cmd);
        write_data(cmdList, numArgs);
        cmdList += numArgs;

        if (ms) {
            ms = *cmdList++;
            if (ms == 255) ms = 500;
            delay_ms(ms);
        }
    }
}

static const uint8_t init_cmds[] = {
    15,                             // 15 commands in list
    ST7735_SWRESET, 0x80,          // 1: Software reset, 0 args, w/delay
    150,                            //    150 ms delay
    ST7735_SLPOUT, 0x80,           // 2: Out of sleep mode, 0 args, w/delay
    255,                            //    500 ms delay
    ST7735_FRMCTR1, 3,             // 3: Frame rate ctrl, 3 args
    0x01, 0x2C, 0x2D,              //    Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3,             // 4: Frame rate ctrl, 3 args
    0x01, 0x2C, 0x2D,              //    Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6,             // 5: Frame rate ctrl, 6 args
    0x01, 0x2C, 0x2D,              //    Dot inversion mode
    0x01, 0x2C, 0x2D,              //    Line inversion mode
    ST7735_INVCTR, 1,              // 6: Display inversion ctrl, 1 arg
    0x07,                           //    No inversion
    ST7735_PWCTR1, 3,              // 7: Power control, 3 args
    0xA2,
    0x02,                           //    -4.6V
    0x84,                           //    AUTO mode
    ST7735_PWCTR2, 1,              // 8: Power control, 1 arg
    0xC5,                           //    VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3, 2,              // 9: Power control, 2 args
    0x0A,                           //    Opamp current small
    0x00,                           //    Boost frequency
    ST7735_PWCTR4, 2,              // 10: Power control, 2 args
    0x8A,                           //    BCLK/2, Opamp current small & Medium low
    0x2A,
    ST7735_PWCTR5, 2,              // 11: Power control, 2 args
    0x8A, 0xEE,
    ST7735_VMCTR1, 1,              // 12: Power control, 1 arg
    0x0E,
    ST7735_INVOFF, 0,              // 13: Don't invert display
    ST7735_MADCTL, 1,              // 14: Memory access control
    0xC8,                           //    row addr/col addr, bottom to top refresh
    ST7735_COLMOD, 1,              // 15: Set color mode, 1 arg
    0x05                            //    16-bit color
};

esp_err_t st7735_init(uint8_t init_type) {
    ESP_LOGI(TAG, "Initializing ST7735 display");

    // Configure GPIO pins
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << TFT_DC) | (1ULL << TFT_RST) | (1ULL << TFT_CS),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // Configure backlight pin
    gpio_config_t bl_conf = {
        .pin_bit_mask = (1ULL << TFT_BL),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&bl_conf);
    gpio_set_level(TFT_BL, 1); // Turn on backlight

    // Configure SPI bus (software SPI using MOSI and SCLK)
    spi_bus_config_t buscfg = {
        .mosi_io_num = TFT_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = TFT_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = ST7735_WIDTH * ST7735_HEIGHT * 2,
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000,  // 10 MHz
        .mode = 0,
        .spics_io_num = TFT_CS,
        .queue_size = 7,
        .flags = SPI_DEVICE_NO_DUMMY,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi));

    // Hardware reset
    gpio_set_rst(1);
    delay_ms(10);
    gpio_set_rst(0);
    delay_ms(10);
    gpio_set_rst(1);
    delay_ms(10);

    // Initialize display
    common_init(init_cmds);

    // Black tab specific settings
    if (init_type == INITR_BLACKTAB) {
        _xstart = 0;
        _ystart = 0;
    }

    write_command(ST7735_NORON);
    delay_ms(10);

    write_command(ST7735_DISPON);
    delay_ms(10);

    ESP_LOGI(TAG, "ST7735 initialized successfully");
    return ESP_OK;
}

void st7735_set_rotation(uint8_t rotation) {
    _rotation = rotation % 4;
    write_command(ST7735_MADCTL);

    uint8_t madctl = 0;
    switch (_rotation) {
        case 0:
            madctl = MADCTL_MX | MADCTL_MY | MADCTL_RGB;
            _width = ST7735_WIDTH;
            _height = ST7735_HEIGHT;
            _xstart = 0;
            _ystart = 0;
            break;
        case 1:
            madctl = MADCTL_MY | MADCTL_MV | MADCTL_RGB;
            _width = ST7735_HEIGHT;
            _height = ST7735_WIDTH;
            _xstart = 0;
            _ystart = 0;
            break;
        case 2:
            madctl = MADCTL_RGB;
            _width = ST7735_WIDTH;
            _height = ST7735_HEIGHT;
            _xstart = 0;
            _ystart = 0;
            break;
        case 3:
            madctl = MADCTL_MX | MADCTL_MV | MADCTL_RGB;
            _width = ST7735_HEIGHT;
            _height = ST7735_WIDTH;
            _xstart = 0;
            _ystart = 0;
            break;
    }
    write_data_byte(madctl);
}

void st7735_fill_screen(uint16_t color) {
    st7735_fill_rect(0, 0, _width, _height, color);
}

void st7735_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (x >= _width || y >= _height || w <= 0 || h <= 0) return;
    if (x + w > _width) w = _width - x;
    if (y + h > _height) h = _height - y;

    set_addr_window(x, y, x + w - 1, y + h - 1);

    uint8_t hi = color >> 8, lo = color & 0xFF;
    gpio_set_dc(1);

    for (int32_t i = w * h; i > 0; i--) {
        spi_write_byte(hi);
        spi_write_byte(lo);
    }
}

void st7735_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    // Top
    st7735_fill_rect(x, y, w, 1, color);
    // Bottom
    st7735_fill_rect(x, y + h - 1, w, 1, color);
    // Left
    st7735_fill_rect(x, y, 1, h, color);
    // Right
    st7735_fill_rect(x + w - 1, y, 1, h, color);
}

void st7735_draw_char(int16_t x, int16_t y, char c, uint16_t color, uint16_t bg, uint8_t size) {
    if (c < 32 || c > 127) return;

    for (int8_t i = 0; i < 5; i++) {
        uint8_t line = font5x7[(c - 32) * 5 + i];
        for (int8_t j = 0; j < 8; j++, line >>= 1) {
            if (line & 1) {
                if (size == 1) {
                    set_addr_window(x + i, y + j, x + i, y + j);
                    uint8_t data[] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
                    gpio_set_dc(1);
                    write_data(data, 2);
                } else {
                    st7735_fill_rect(x + i * size, y + j * size, size, size, color);
                }
            } else if (bg != color) {
                if (size == 1) {
                    set_addr_window(x + i, y + j, x + i, y + j);
                    uint8_t data[] = {(uint8_t)(bg >> 8), (uint8_t)(bg & 0xFF)};
                    gpio_set_dc(1);
                    write_data(data, 2);
                } else {
                    st7735_fill_rect(x + i * size, y + j * size, size, size, bg);
                }
            }
        }
    }
}

void st7735_draw_string(int16_t x, int16_t y, const char* str, uint16_t color, uint16_t bg, uint8_t size) {
    int16_t cursor_x = x;
    while (*str) {
        st7735_draw_char(cursor_x, y, *str, color, bg, size);
        cursor_x += 6 * size;
        str++;
    }
}

int16_t st7735_width(void) {
    return _width;
}

int16_t st7735_height(void) {
    return _height;
}

void st7735_draw_image(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *data) {
    if (x >= _width || y >= _height || w <= 0 || h <= 0) return;
    if (x + w > _width) w = _width - x;
    if (y + h > _height) h = _height - y;

    set_addr_window(x, y, x + w - 1, y + h - 1);
    gpio_set_dc(1);

    // Send all image data at once
    size_t data_size = w * h * 2; // 2 bytes per pixel (RGB565)
    write_data(data, data_size);
}
