#include "stm32f10x.h"
#include "color_led.h"

void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

int main(void)
{			
	TIMx_Init();
	while(1)
	{	
		SetColorValue(0,0,0);
		Delay(0xFFFFFF);
		SetColorValue(255,255,255);
		Delay(0xFFFFFF);
		SetColorValue(255,0,0);
		Delay(0xFFFFFF);
		SetColorValue(0,255,0);
		Delay(0xFFFFFF);
		SetColorValue(0,0,255);
		Delay(0xFFFFFF);
		SetColorValue(255,255,0);
		Delay(0xFFFFFF);
		SetColorValue(0,255,255);
		Delay(0xFFFFFF);
		SetColorValue(255,0,255);
		Delay(0xFFFFFF);
		SetColorValue(192,192,192);
		Delay(0xFFFFFF);
		SetColorValue(128,128,128);
		Delay(0xFFFFFF);
		SetColorValue(128,0,0);
		Delay(0xFFFFFF);
	}
}
