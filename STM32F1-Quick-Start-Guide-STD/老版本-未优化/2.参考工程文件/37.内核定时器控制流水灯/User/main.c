#include "stm32f10x.h"
#include "led.h"
#include "core_delay.h"   

int main(void)
{	
	LED_Init();
	CPU_TS_TmrInit();
	while(1)
	{
		LED1_ON(); 
	    Delay_us(100000); // 100000 * 10us = 1000ms
		LED1_OFF();
		LED2_ON();
	    Delay_us(100000);		
		LED2_OFF();
		LED3_ON();
	    Delay_us(100000);		
		LED3_OFF();
	} 
}
