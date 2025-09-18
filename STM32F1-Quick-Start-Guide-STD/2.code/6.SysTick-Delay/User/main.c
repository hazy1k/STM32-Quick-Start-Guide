#include "stm32f10x.h"
#include "led.h"
#include "SysTick.h"

int main()
{
	BSP_LED_Init();
	SysTick_Init();
	while(1)
	{
		LED_ON(RED_LED_Pin);
		Delay_ms(1000);
		LED_OFF(RED_LED_Pin);
		Delay_ms(1000);
		LED_ON(GREEN_LED_Pin);
		Delay_ms(1000);
		LED_OFF(GREEN_LED_Pin);
		Delay_ms(1000);
	}
}
