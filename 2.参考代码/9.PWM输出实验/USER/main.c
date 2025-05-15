#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"

int main(void)
{
	uint8_t dir = 1; // 1: ���� 0: ����
	uint16_t led0_duty = 0; // LED0 PWM���ռ�ձ� 
	HAL_Init();
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);
	uart_init(115200);
	LED_Init();
	// Time = (500*84)/84MHz = 500ms
	// ����Ƶ��Ϊ84MHz/psc(84) = 1MHz
	// PWMƵ��Ϊ1MHz/500 = 2KHz
	TIM14_PWM_Init(500-1, 84-1);
	while(1)
	{
		delay_ms(10);
		if(dir)
		{
			led0_duty++; // �����ʱ������ռ�ձ�
		}
		else
		{
			led0_duty--; // �����ʱ�򣬼���ռ�ձ�
		}
		if(led0_duty > 300) // ռ�ձȴﵽ��ֵ���ı䷽��
		{
			dir = 0;
		}
		if(led0_duty == 0) // ռ�ձȴﵽ��ֵ���ı䷽��
		{
			dir = 1;
		}
		TIM_SetTIM14Compare1(led0_duty);
	}
}
