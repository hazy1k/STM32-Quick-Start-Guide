#include "stm32f10x.h"
#include "led.h"

#define led_delay delay(0x0FFFFF);

void delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

int main()
{
	BSP_LED_Init();
	while(1)
	{
		LED_ON(RED_LED_Pin);
		led_delay;
		LED_OFF(RED_LED_Pin);
		led_delay;
		LED_ON(GREEN_LED_Pin);
		led_delay;
		LED_OFF(GREEN_LED_Pin);
		led_delay;
		LED_ON(BLUE_LED_Pin);
		led_delay;
		LED_OFF(BLUE_LED_Pin);
		led_delay;
	}
}
