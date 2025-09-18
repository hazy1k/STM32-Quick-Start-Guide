#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "usmart.h"
#include "wkup.h"

int main(void)
{
    HAL_Init();                  // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7); // 设置时钟,168Mhz
	delay_init(168);             // 初始化延时函数
	uart_init(115200);           // 初始化USART
	usmart_dev.init(84); 		 // 初始化USMART	
	LED_Init();				     // 初始化LED	
 	LCD_Init();           	     // 初始化LCD
	WKUP_Init();	             // 待机唤醒初始化
	POINT_COLOR = RED; // 设置字体颜色为红色 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"WKUP TEST");	
	LCD_ShowString(30,130,200,16,16,"WK_UP:Stanby/WK_UP");	   
	while(1)
	{
        LED0=!LED0;
		delay_ms(250);              //延时250ms
	} 
}
