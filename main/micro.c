#include "led.h"
#include "system_info.h"
#include "st7735.h"
#include "image_data.h"
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

    // Display the image (160x128 pixels, fills entire screen in landscape mode)
    st7735_draw_image(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, image_data);

    // Blink LED forever (blocks)
    led_blink_forever();
}
