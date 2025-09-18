#include "bsp_init.h"

int main(void)
{
    // iwdg预分频器4（64倍分频），重载值500
    // time = ((4*2^4)*500)/40 = 800ms
    bsp_init();
    LED_ON(LED0_GPIO_Pin); // 默认开启LED0，iwdg会复位
    printf("iwdg reset !!!\r\n");
    while(1)
    {
        if(key_scan(0) == WKUP_Press)
        {
            printf("key feed !!!\r\n");
            iwdg_feed();
            LED_ON(LED1_GPIO_Pin);
        }
    }
}
