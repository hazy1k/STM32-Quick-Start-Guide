#include "stm32f10x.h"
#include "led.h"
#include "key.h"

int main()
{
	BSP_LED_Init();
	BSP_KEY_Init();
	while(1)
	{
		if(KEY_Scan(KEY1_GPIO_Port, KEY1_GPIO_Pin) == KEY_ON)
		{
			LED_ON(RED_LED_Pin);
		}
		if(KEY_Scan(KEY2_GPIO_Port, KEY2_GPIO_Pin) == KEY_ON)
		{
			LED_OFF(RED_LED_Pin);
		}
	}
}
