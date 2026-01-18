#include "st7735.h"

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
}
