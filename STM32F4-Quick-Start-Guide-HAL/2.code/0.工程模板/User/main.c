#include "bsp_init.h"

int main(void)
{
    bsp_init();
    LED_ON(LED0_GPIO_Pin|LED1_GPIO_Pin);
    while(1);
}
