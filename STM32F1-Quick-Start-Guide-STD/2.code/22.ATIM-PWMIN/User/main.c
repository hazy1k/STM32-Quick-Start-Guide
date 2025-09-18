#include "stm32f10x.h"
#include "tim.h"
#include "systick.h"
#include "usart.h"
#include <stdio.h>

int main(void)
{   
    SysTick_Init();
    USARTx_Init(115200);
    printf("TIM2产生PWM信号 -> TIM1捕获PWM参数\r\n");
    printf("PA0 (PWM输出) 连接到 PA8 (PWM输入)\r\n");
    TIM2_PWM_Init();
    TIM1_PWMCapture_Init();
    /* PWM参数设置 */
    uint32_t periods[] = {1000, 2000, 5000};  // 不同周期值
    uint32_t duties[] = {200, 500, 800};      // 不同占空比值
    uint8_t p_index = 0, d_index = 0;
    printf("开始PWM捕获测试...\r\n");
    while(1)
    {
        TIM2_SetPWM(periods[p_index], duties[d_index]);
        // 打印设置的PWM参数
        printf("设置的PWM参数: 周期=%lu, 占空比=%lu (%.1f%%)\r\n", 
               periods[p_index], duties[d_index],
               (duties[d_index] * 100.0f) / periods[p_index]);
        Delay_ms(1000);
        // 获取捕获的PWM数据
        PWM_CaptureData cap_data = TIM1_GetPWMCaptureData();
        // 打印捕获结果
        printf("捕获的PWM参数:\r\n");
        printf("  周期: %lu 计数\r\n", cap_data.period);
        printf("  占空比: %lu 计数\r\n", cap_data.duty_cycle);
        printf("  频率: %.2f Hz\r\n", cap_data.frequency);
        printf("  占空比: %.2f%%\r\n", cap_data.duty_ratio);
        printf("------------------------------------\r\n");
        // 更新PWM参数索引
        d_index = (d_index + 1) % 3;
        if (d_index == 0) {
            p_index = (p_index + 1) % 3;
        }
        Delay_ms(2000);
    }
}
