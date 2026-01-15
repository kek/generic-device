#include "led.h"
#include "system_info.h"

void app_main(void)
{
    led_init();
    system_info_print();
    led_blink_forever();
}
