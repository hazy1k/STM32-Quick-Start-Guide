#include <bsp_init.h>
#include <atim.h>
#include <stdio.h>

int main(void) {
    uint8_t key_value = 0;
    uint8_t i = 0;
    GPIO_InitTypeDef GPIO_InitStructure;
    bsp_init();
    GPIO_InitStructure.Pin = LED1_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStructure);
    tim8_pwm_init(10000-1, 8400-1);
    TIM8->CCR1 = 5000; // 占空比为50%
    tim8_set_pulse_count(5); // 设置脉冲数为5
    while(1)
    {
        key_value = key_scan(0);
        if(key_value == KEY0_Press)
        {
            tim8_set_pulse_count(10); // 设置脉冲数为10
        }
        i++;
        delay_ms(10);
        if(i>50)
        {
            i=0;
            LED_TOGGLE(LED0_GPIO_Pin);
        }
    }
}
