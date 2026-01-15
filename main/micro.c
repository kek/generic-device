#include "led.h"
#include "system_info.h"
#include "st7735.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    // Initialize LED
    led_init();

    // Print system info
    system_info_print();

    // Initialize ST7735 display
    st7735_init(INITR_BLACKTAB);

    // Set rotation to landscape (matching Arduino setRotation(1))
    st7735_set_rotation(1);

    // Fill screen with black
    st7735_fill_screen(ST77XX_BLACK);

    // Draw text "<3 LM" in white, size 2
    st7735_draw_string(15, 50, "<3 LM", ST77XX_WHITE, ST77XX_BLACK, 2);

    // Draw yellow rectangle around the text
    st7735_draw_rect(10, 40, 145, 40, ST77XX_YELLOW);

    // Blink LED forever (blocks)
    led_blink_forever();
}
