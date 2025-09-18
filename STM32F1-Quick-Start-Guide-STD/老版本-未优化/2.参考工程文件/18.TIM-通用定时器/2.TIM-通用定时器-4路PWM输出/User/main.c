// TIM—通用定时器-4路PWM输出应用
#include "stm32f10x.h"
#include "led.h"
#include "GeneralTim.h"  

int main(void)
{
	LED_Init();
	LED_BLUE();
	GeneralTim_Init();
	while(1)
	{      
	}
}
