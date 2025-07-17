#include <bsp_init.h>
#include <atim.h>
#include <stdio.h>

int main(void) {
    bsp_init();
    TIM8_PWM_Init(1000-1, 168-1);
    TIM8_CH1_CCRx = 250 - 1; /* ͨ�� 1 ��λ 25% */
    TIM8_CH2_CCRx = 500 - 1; /* ͨ�� 2 ��λ 50% */
    TIM8_CH3_CCRx = 750 - 1; /* ͨ�� 3 ��λ 75% */
    TIM8_CH4_CCRx = 999 - 1; /* ͨ�� 4 ��λ 99% */
    while(1) {
        LED_TOGGLE(LED0_GPIO_Pin);
        HAL_Delay(500);
        
        // ��Ӵ�����������ȷ�ϳ�������
        static uint32_t counter = 0;
        if((counter++ % 10) == 0) {
            printf("System running... %lus\r\n", counter/2);
        }
    }
}
