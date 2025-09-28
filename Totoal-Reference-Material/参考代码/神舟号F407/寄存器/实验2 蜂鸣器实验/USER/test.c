#include "sys.h"
#include "delay.h" 
#include "led.h"
#include "beep.h"
//ALIENTEK 探索者STM32F407开发板 实验2
//蜂鸣器实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
  
int main(void)
{ 
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz
	delay_init(168);		//初始化延时函数
	LED_Init();				//初始化LED时钟  
	BEEP_Init();         	//初始化蜂鸣器端口
	while(1)
	{
		LED0=0;
		BEEP=0;		  
		delay_ms(300);
		LED0=1;	  
		BEEP=1;  
		delay_ms(300);
	}	
}

















