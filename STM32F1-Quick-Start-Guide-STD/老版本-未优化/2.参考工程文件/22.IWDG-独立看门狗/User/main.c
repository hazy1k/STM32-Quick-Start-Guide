#include "key.h"
#include "led.h"
#include "iwdg.h"

int main(void)
{
	LED_Init();
	Key_Init();
	IWDG_Config(IWDG_Prescaler_64 , 625); // IWDG 1s timeout
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET) // Check if the reset was caused by IWDG reset
	{
		LED_RED();
		RCC_ClearFlag(); 
	}
	else
	{
		LED_BLUE();
	}
	while(1)
	{
		if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PING) == KEY_ON)
		{
			IWDG_Feed();
			LED_GREEN();
		}
	}
}
