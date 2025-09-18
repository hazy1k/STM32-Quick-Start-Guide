#include "stm32f10x.h"
#include "beep.h"

#define beep_delay delay(0x0FFFFF);

void delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

int main()
{
	BSP_Beep_Init();
	while(1)
	{
		BEEP_ON();
		beep_delay;
		BEEP_OFF();
		beep_delay;
	}
}
