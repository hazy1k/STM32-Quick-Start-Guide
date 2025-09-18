#include "bsp_init.h"

int main(void)
{
    bsp_init();
    while(1)
    {
        LED_TOGGLE(LED0_GPIO_Pin);
        LED_TOGGLE(LED1_GPIO_Pin);
        delay_ms(500);
    }
}
