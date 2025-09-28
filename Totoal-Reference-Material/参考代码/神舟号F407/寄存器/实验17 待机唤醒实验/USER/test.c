#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h" 
#include "wkup.h" 
//ALIENTEK 探索者STM32F407开发板 实验17
//待机唤醒 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司

int main(void)
{       
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(84,115200);		//初始化串口波特率为115200   
	LED_Init();					//初始化LED  
 	WKUP_Init();				//待机唤醒初始化
 	LCD_Init(); 				//初始化LCD
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"WKUP TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/5/6");	
	LCD_ShowString(30,130,200,16,16,"WK_UP:Stanby/WK_UP");	 
	while(1)
	{
		LED0=!LED0;
		delay_ms(250);  
	}
}

















