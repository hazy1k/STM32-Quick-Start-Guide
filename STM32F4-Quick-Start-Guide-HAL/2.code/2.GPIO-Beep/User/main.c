#include "bsp_init.h"

int main(void)
{
    bsp_init();
    while(1)
    {
        LED_ON(LED0_GPIO_Pin);
        BEEP_ON();
        delay_ms(500);
        BEEP_OFF();
        LED_OFF(LED0_GPIO_Pin);
        delay_ms(500);
    }
}
