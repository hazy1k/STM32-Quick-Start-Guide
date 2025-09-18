#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "adc.h"

int main(void)
{
	short temp; 
	
    HAL_Init();                  // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7); // 设置时钟,168Mhz
	delay_init(168);             // 初始化延时函数
	uart_init(115200);           // 初始化USART
	usmart_dev.init(84); 		 // 初始化USMART
	LED_Init();				     // 初始化LED	
	KEY_Init();				     // 初始化KEY
 	LCD_Init();           		 // 初始化LCD
    MY_ADC_Init();               // 初始化ADC1
	POINT_COLOR = RED; // 设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"Temperature TEST");  
	POINT_COLOR=BLUE;//设置字体为蓝色      
	LCD_ShowString(30,140,200,16,16,"TEM PERATE: 00.00C");//先在固定位置显示小数点	
    while(1)
	{
		temp = Get_Temprate(); // 得到温度值 
		if(temp < 0)
		{
			temp =- temp; // 显示负数
			LCD_ShowString(30+10*8,140,16,16,16,"-");	// 显示负号
		}else LCD_ShowString(30+10*8,140,16,16,16," ");	// 无符号
		LCD_ShowxNum(30+11*8,140,temp/100,2,16,0);		//显示整数部分
		LCD_ShowxNum(30+14*8,140,temp%100,2,16,0);		//显示小数部分 
		LED0 = !LED0;
		delay_ms(250);	
	} 
}
