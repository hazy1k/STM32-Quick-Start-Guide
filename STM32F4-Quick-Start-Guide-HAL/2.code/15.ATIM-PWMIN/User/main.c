#include <bsp_init.h>
#include <atim.h>
#include <stdio.h>

/* 状态变量 0,无，1，有 */
extern uint8_t  PWMIN_STA;// PWM输入状态
extern uint16_t PWMIN_PSC;// PWM输入分频系数
extern uint32_t PWMIN_Highvlaue; // PWM输入高电平时间
extern uint32_t PWMIN_CycleTime; // PWM输入周期时间

int main(void) 
{
    uint8_t t = 0;
    double High_time, Cycle_time, PWM_Freq, PWM_PSC;

    bsp_init();
    // 先初始化PWM输出
    // 100kHz PWM: 84MHz/((84-1+1)*(10-1+1)) = 84,000,000 / 840 = 100,000 Hz
    tim14_pwmout_init(10-1, 84-1);
    // 设置20%占空比 (周期10个计数单位，高电平2个)
    TIM14->CCR1 = 2;
    // 初始化PWM输入捕获
    tim8_pwmin_init();
    printf("PWM Capture Demo Started\r\n");
    while(1)
    {
        delay_ms(10);
        t++;
        if(t > 100)
        {
            if(PWMIN_STA)
            {
                printf("\r\n");
                /* 直接打印捕获值 */
                printf("PWM PSC       :%d\r\n", PWMIN_PSC);
                printf("PWM Highvlaue :%d\r\n", PWMIN_Highvlaue);
                printf("PWM Cyclevlaue:%d\r\n", PWMIN_CycleTime);
                /* 计算 */
                PWM_PSC = ((double)PWMIN_PSC+1)/168;
                High_time = PWMIN_Highvlaue * PWM_PSC;
                Cycle_time = PWMIN_CycleTime * PWM_PSC;
                PWM_Freq = (1/Cycle_time)*1000000;
                printf("PWM High Time :%.3fus\r\n", High_time);
                printf("PWM Cycle Time:%.3fus\r\n", Cycle_time);
                printf("PWM Duty      :%.2f%%\r\n", (High_time/Cycle_time)*100);
                printf("PWM Freq      :%.3fHz\r\n", PWM_Freq);
                restart_pwmin_capture();
            }
            LED_TOGGLE(LED1_GPIO_Pin);
            t = 0;
        }
    }
}
