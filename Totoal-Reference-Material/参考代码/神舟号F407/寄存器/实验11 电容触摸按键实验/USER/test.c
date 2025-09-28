#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "tpad.h"
//ALIENTEK 探索者STM32F407开发板 实验11
//电容触摸按键实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
 
int main(void)
{   
	u8 t=0;
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(84,115200);		//初始化串口波特率为115200 
	LED_Init();					//初始化LED
 	TPAD_Init(8);				//初始化触摸按键,以84/4=21Mhz频率计数
   	while(1)
	{					  						  		 
 		if(TPAD_Scan(0))	//成功捕获到了一次上升沿(此函数执行时间至少15ms)
		{
			LED1=!LED1;		//LED1取反
		}
		t++;
		if(t==15)		 
		{
			t=0;
			LED0=!LED0;		//LED0取反,提示程序正在运行
		}
		delay_ms(10);
	}
}

















