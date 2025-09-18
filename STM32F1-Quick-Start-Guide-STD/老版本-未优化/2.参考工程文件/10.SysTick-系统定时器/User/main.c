#include "stm32f10x.h"
#include "SysTick.h"
#include "led.h"

int main()
{
	LED_Init();
	SysTick_Init();
	while(1)
	{
		LED1_ON();
		Delay_ms(1000);
		LED1_OFF();
		LED2_ON();
		Delay_ms(1000);
		LED2_OFF();
		LED3_ON();
		Delay_ms(1000);
		LED3_OFF();
	}
}
