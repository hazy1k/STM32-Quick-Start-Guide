#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "lcd.h"
#include "usmart.h"
#include "rtc.h"
//ALIENTEK 探索者STM32F407开发板 实验15
//RTC 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
    
int main(void)
{    
	u8 hour,min,sec,ampm;
	u8 year,month,date,week;
	u8 tbuf[40];
	u8 t=0;
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(84,115200);		//初始化串口波特率为115200 
	usmart_dev.init(84); 		//初始化USMART	
	LED_Init();					//初始化LED
 	LCD_Init();					//初始化LCD
	RTC_Init();		 			//初始化RTC
	RTC_Set_WakeUp(4,0);		//配置WAKE UP中断,1秒钟中断一次 
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"RTC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/5/5");	
  	while(1) 
	{		
		t++;
		if((t%10)==0)	//每100ms更新一次显示数据
		{
			RTC_Get_Time(&hour,&min,&sec,&ampm);
			sprintf((char*)tbuf,"Time:%02d:%02d:%02d",hour,min,sec); 
			LCD_ShowString(30,140,210,16,16,tbuf);	
			RTC_Get_Date(&year,&month,&date,&week);
			sprintf((char*)tbuf,"Date:20%02d-%02d-%02d",year,month,date); 
			LCD_ShowString(30,160,210,16,16,tbuf);	
			sprintf((char*)tbuf,"Week:%d",week); 
			LCD_ShowString(30,180,210,16,16,tbuf);
		} 
		if((t%20)==0)LED0=!LED0;	//每200ms,翻转一次LED0 
		delay_ms(10);
	}	
}

















