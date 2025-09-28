#include "sys.h"
#include "delay.h"  
#include "led.h"
#include "timer.h"
//ALIENTEK 探索者STM32F407开发板 实验8
//定时器中断实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
  
int main(void)
{  
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	LED_Init();		  			//初始化LED
 	TIM3_Int_Init(5000-1,8400-1);//10Khz的计数频率，计数5K次为500ms     
	while(1)
	{
		LED0=!LED0;
		delay_ms(200);
	};
}
