#include "bsp_init.h"
#include "gtim.h"
#include <stdio.h>

int main(void)
{
    // 1. 初始化硬件
    bsp_init();
    printf("Pulse Counter Started\r\n");
    
    // 2. 初始化定时器（不分频）
    TIM2_Mode_Init(0);
    TIM2_CH1_Restart();
    
    // 3. 主循环变量
    uint32_t last_count = 0;
    uint8_t led_blink = 0;
    
    while(1)
    {
        // 按键处理：KEY0按下时重启计数器
        if(key_scan(0) == KEY0_Press)
        {
            printf("Counter Reset\r\n");
            TIM2_CH1_Restart();
            last_count = 0; // 重置上次计数值
        }
        
        // 获取当前计数值（约每20ms一次）
        uint32_t current_count = TIM2_CH1_GetCount();
        
        // 检测计数值变化
        if(current_count != last_count)
        {
            printf("CNT: %lu\r\n", current_count);
            last_count = current_count;
        }
        
        // LED闪烁指示系统运行（约400ms周期）
        if(++led_blink >= 20)
        {
            led_blink = 0;
            LED_TOGGLE(LED0_GPIO_Pin);
        }
        
        delay_ms(20);
    }
}
