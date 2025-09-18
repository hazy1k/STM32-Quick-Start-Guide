#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "usmart.h"
#include "rtc.h"

int main(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    u8 tbuf[40];
	u8 t=0;
	
    HAL_Init();                  // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7); // 设置时钟,168Mhz
	delay_init(168);             // 初始化延时函数
	uart_init(115200);           // 初始化USART
	LED_Init();				     // 初始化LED	
 	LCD_Init();           	     // 初始化LCD
	usmart_dev.init(84); 		 // 初始化USMART	
    RTC_Init();                  // 初始化RTC 
    RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0); // 配置WAKE UP中断,1秒钟中断一次  
    POINT_COLOR=RED; // 设置背景色为红色
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"RTC TEST");	
	LCD_ShowString(30,90,200,16,16,"by hazy1k");
	LCD_ShowString(30,110,200,16,16,"2024/10/11");		   
    while(1)
    {
		t++;
		if((t%10)==0) // 每100ms更新一次显示数据
		{
            HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN); // 获取当前时间
			sprintf((char*)tbuf,"Time:%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); // 打印时间
			LCD_ShowString(30,140,210,16,16,tbuf); // 显示时间
            HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN); // 获取当前日期
			sprintf((char*)tbuf,"Date:20%02d-%02d-%02d",RTC_DateStruct.Year,RTC_DateStruct.Month,RTC_DateStruct.Date); // 打印日期 
			LCD_ShowString(30,160,210,16,16,tbuf); // 显示日期
			sprintf((char*)tbuf,"Week:%d",RTC_DateStruct.WeekDay); // 打印星期
			LCD_ShowString(30,180,210,16,16,tbuf); // 显示星期
		} 
		if((t%20)==0)LED0=!LED0; // 每200ms,翻转一次LED0 
        delay_ms(10);
	}
}