#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

int main(void)
{ 
    HAL_Init();                 // ��ʼ��HAL��,��Դ��"stm32f4xx_hal.c" 
    // ����ʱ��,168Mhz,��Դ��"sys.c" ����������plln��pllm��pllp��pllq
    // ������Ϊ8M��ʱ��,�Ƽ�ֵ:plln=336,pllm=8,pllp=2,pllq=7.
    // �õ�:Fvco=8*(336/8)=336Mhz
    //     SYSCLK=336/2=168Mhz
    //     Fusb=336/7=48Mhz
    Stm32_Clock_Init(336,8,2,7); // ϵͳʱ�ӳ�ʼ��
	delay_init(168);             // ��ʼ����ʱ����,168Mhzϵͳʱ��
	LED_Init();					 // ��ʼ��LED	
	while(1)
	{
        // ��LED0����LED1
        HAL_GPIO_WritePin(LED0_GPIO, LED0_PIN, GPIO_PIN_RESET);  // ����LED0
        HAL_GPIO_WritePin(LED1_GPIO, LED1_PIN, GPIO_PIN_SET);    // Ϩ��LED1
        delay_ms(500);  // ��ʱ500ms
        // ��LED0����LED1
        HAL_GPIO_WritePin(LED0_GPIO, LED0_PIN, GPIO_PIN_SET);    // Ϩ��LED0
        HAL_GPIO_WritePin(LED1_GPIO, LED1_PIN, GPIO_PIN_RESET);  // ����LED1
        delay_ms(500);  // ��ʱ500ms             
	}
}
