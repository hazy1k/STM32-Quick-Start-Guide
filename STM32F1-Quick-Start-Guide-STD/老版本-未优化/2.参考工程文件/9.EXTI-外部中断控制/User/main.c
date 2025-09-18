#include "stm32f10x.h"
#include "led.h"
#include "exti.h" 

int main(void)
{
	LED_Init();
	EXTI_KeyInit();
	while(1)                            
	{

	}
}
