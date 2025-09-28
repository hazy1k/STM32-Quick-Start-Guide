#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h" 
#include "adc.h"
//ALIENTEK 探索者STM32F407开发板 实验19
//内部温度传感器 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
    
int main(void)
{       
	short temp; 
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(84,115200);		//初始化串口波特率为115200   
	LED_Init();					//初始化LED 
 	LCD_Init();					//初始化LCD
	Adc_Init(); 				//初始化ADC
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"Temperature TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/5/6");	  
	POINT_COLOR=BLUE;//设置字体为蓝色      
	LCD_ShowString(30,140,200,16,16,"TEMPERATE: 00.00C");	      
	while(1)
	{
		temp=Get_Temprate();	//得到温度值 
		if(temp<0)
		{
			temp=-temp;
			LCD_ShowString(30+10*8,140,16,16,16,"-");	//显示负号
		}else LCD_ShowString(30+10*8,140,16,16,16," ");	//无符号
		
		LCD_ShowxNum(30+11*8,140,temp/100,2,16,0);		//显示整数部分
		LCD_ShowxNum(30+14*8,140,temp%100,2,16,0X80);	//显示小数部分 
		 
		LED0=!LED0;
		delay_ms(250);	
	}
}

















