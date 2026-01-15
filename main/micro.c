#include "st7735.h"
#include "space_invaders.h"

void app_main(void)
{
    // Initialize ST7735 display
    st7735_init(INITR_BLACKTAB);

    // Set rotation to landscape (matching Arduino setRotation(1))
    st7735_set_rotation(1);

    // Run Space Invaders demo
    space_invaders_demo();
}
