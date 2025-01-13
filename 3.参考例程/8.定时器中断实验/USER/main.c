#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "led.h"

int main(void)
{
	HAL_Init();
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);
	uart_init(115200);
	LED_Init();
	// ��ʱ����ʼ����������װֵ(arr)Ϊ5000-1����Ƶϵ��(psc)Ϊ8400-1
	// ������������ǵĶ�ʱ���Ĺ���Ƶ��Ϊ84MHz/8400=10KHz
	// ���ڼ���Time = ((arr+1)*(psc+1))/fclk = 5000*(8400)/84000000 = 500ms
	TIM_Config(5000-1,8400-1);
	while(1)
	{
		LED0 =! LED0;
		delay_ms(200);	
	}	
}
// LED0ÿ200ms��תһ��
// LED1ͨ����ʱ���ж�ÿ��500ms��תһ��
