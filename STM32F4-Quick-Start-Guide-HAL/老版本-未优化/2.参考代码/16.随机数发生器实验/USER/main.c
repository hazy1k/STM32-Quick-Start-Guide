#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "rng.h"

int main(void)
{
    u32 random;
    u8 t=0,key;
	
    HAL_Init();                 // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);// 设置时钟,168Mhz
	delay_init(168);            // 初始化延时函数
	uart_init(115200);          // 初始化USART
	LED_Init();				    // 初始化LED	
	KEY_Init();				    // 初始化KEY
 	LCD_Init();           		// 初始化LCD
	usmart_dev.init(84); 		// 初始化USMART	
	POINT_COLOR = RED; // 设置字体颜色为红色	
    while(RNG_Init())	 		    //初始化随机数发生器
	{
		LCD_ShowString(30,130,200,16,16,"  RNG Error! ");	 
		delay_ms(200);
		LCD_ShowString(30,130,200,16,16,"RNG Trying...");	 
	}                                 
	LCD_ShowString(30,130,200,16,16,"RNG Ready!   ");	 
	LCD_ShowString(30,150,200,16,16,"KEY0:Get Random Num");	 
	LCD_ShowString(30,180,200,16,16,"Random Num:");	 
	LCD_ShowString(30,210,200,16,16,"Random Num[0-9]:"); 	   
    while(1)
    {
		key = KEY_Scan(0); // 检测按键
		if(key==KEY0_PRES) // 按键KEY0被按下
		{
			random = RNG_Get_RandomNum(); //获得随机数
			LCD_ShowNum(30+8*11,180,random,10,16); //显示随机数
		} 
		if((t%20)==0) // 每200ms
		{ 
			LED0 = !LED0;	                     // 每200ms,翻转一次LED0 
			random = RNG_Get_RandomRange(0,9);   // 获取[0,9]区间的随机数
			LCD_ShowNum(30+8*16,210,random,1,16);//显示随机数
		 }
		delay_ms(10);
		t++;
	} 
}