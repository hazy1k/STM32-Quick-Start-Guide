// TIM—通用-捕获-电容按键检测 应用
#include "stm32f10x.h"
#include "usart.h" 
#include "SysTick.h"
#include "beep.h"
#include "tpad.h"

int main(void)
{
	SysTick_Init();
	USART_Config();
	BEEP_GPIO_Config();
	TPAD_Init(); 
	while(1)
	{
		if(TPAD_Scan() == TPAD_ON)
		{
			BEEP(ON);
			printf("按下按键\r\n");
			Delay_ms(100);
			BEEP(OFF);
		}
	}
}
