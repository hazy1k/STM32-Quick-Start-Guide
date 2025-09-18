#include "bsp_init.h"
#include "gtim.h"
#include <stdio.h>

extern volatile CapState_t TIM5CH1_CAP;

int main(void)
{
    bsp_init(); 
    TIM5_CH1_Init(0xFFFF, 84-1); 
    uint32_t last_process = 0;

    while(1)
    {
        // 每10ms处理一次捕获数据
        if (HAL_GetTick() - last_process >= 10)
        {
            last_process = HAL_GetTick();
            if (TIM5CH1_CAP.status & CAP_COMPLETE)
            {
                // 计算高电平持续时间 (单位: us)
                uint32_t overflow_count = TIM5CH1_CAP.status & CAP_OVF_MASK;
                uint32_t high_time = (uint32_t)overflow_count * 65536 + TIM5CH1_CAP.cap_val;
                printf("High time: %lu us\r\n", high_time);
                // 重置捕获状态
                TIM5CH1_CAP.status = CAP_IDLE;
            }
            static uint8_t led_counter = 0;
            if (++led_counter >= 50)  // 50 * 10ms = 500ms
            {
                led_counter = 0;
                LED_TOGGLE(LED0_GPIO_Pin);
            }
        }
    }
}
