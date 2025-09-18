#include "bsp_init.h"
#include "atim.h"
#include "stdio.h"

int main(void)
{
    bsp_init();
    printf("System Initialized\r\n");   
    // 电容按键初始化（分频系数2，计数频率=84MHz/2=42MHz）
    if(tpad_init(2))
    {
        printf("TPAD Init Failed!\r\n");
        while(1); // 初始化失败，停机
    }
    uint32_t last_led_toggle = 0;
    while(1)
    {
        if(tpad_scan(0))
        {
            LED_TOGGLE(LED1_GPIO_Pin);
            printf("Touch Detected!\r\n");
        }
        if(HAL_GetTick() - last_led_toggle > 500)
        {
            LED_TOGGLE(LED0_GPIO_Pin);
            last_led_toggle = HAL_GetTick();
        }
        delay_ms(50);
    }
}
