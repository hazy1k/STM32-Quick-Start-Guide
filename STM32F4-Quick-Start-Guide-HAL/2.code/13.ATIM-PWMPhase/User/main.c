#include <bsp_init.h>
#include <atim.h>
#include <stdio.h>

int main(void) {
    bsp_init();
    TIM8_PWM_Init(1000-1, 168-1);
    TIM8_CH1_CCRx = 250 - 1; /* 通道 1 相位 25% */
    TIM8_CH2_CCRx = 500 - 1; /* 通道 2 相位 50% */
    TIM8_CH3_CCRx = 750 - 1; /* 通道 3 相位 75% */
    TIM8_CH4_CCRx = 999 - 1; /* 通道 4 相位 99% */
    while(1) {
        LED_TOGGLE(LED0_GPIO_Pin);
        HAL_Delay(500);
        
        // 添加串口心跳包，确认程序运行
        static uint32_t counter = 0;
        if((counter++ % 10) == 0) {
            printf("System running... %lus\r\n", counter/2);
        }
    }
}
