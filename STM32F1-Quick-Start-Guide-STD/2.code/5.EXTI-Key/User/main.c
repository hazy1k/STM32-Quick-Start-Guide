#include "stm32f10x.h"
#include "led.h"
#include "key.h"

int main()
{
	BSP_LED_Init();
	KEY_EXTI_Init();
	while(1);
}
