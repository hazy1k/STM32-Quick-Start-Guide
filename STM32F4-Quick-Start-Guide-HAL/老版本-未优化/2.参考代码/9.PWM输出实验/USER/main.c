#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"

int main(void)
{
	uint8_t dir = 1; // 1: 正序 0: 逆序
	uint16_t led0_duty = 0; // LED0 PWM输出占空比 
	HAL_Init();
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);
	uart_init(115200);
	LED_Init();
	// Time = (500*84)/84MHz = 500ms
	// 计数频率为84MHz/psc(84) = 1MHz
	// PWM频率为1MHz/500 = 2KHz
	TIM14_PWM_Init(500-1, 84-1);
	while(1)
	{
		delay_ms(10);
		if(dir)
		{
			led0_duty++; // 正序的时候，增加占空比
		}
		else
		{
			led0_duty--; // 逆序的时候，减少占空比
		}
		if(led0_duty > 300) // 占空比达到峰值，改变方向
		{
			dir = 0;
		}
		if(led0_duty == 0) // 占空比达到谷值，改变方向
		{
			dir = 1;
		}
		TIM_SetTIM14Compare1(led0_duty);
	}
}
