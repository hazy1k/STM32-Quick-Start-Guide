#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"

int main(void)
{
	uint8_t key_value;
	HAL_Init();
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);
	LED_Init();
	KEY_Init();
	while(1)
	{
		key_value = KEY_Scan(0);
		switch(key_value)
		{
			case KEY_UP_PRES: // LED0,LED1»¥³â
				LED1 = !LED1;
				LED0 = !LED1;
				break;
			case KEY0_PRES: // LED0,LED1Í¬Ê±·­×ª
				LED0 = !LED0;
				LED1 = !LED1;
				break;
			case KEY1_PRES:
				LED1 = !LED1;
				break;
			case KEY2_PRES:
				LED0 = !LED0;
				break;
		}
	}
}
