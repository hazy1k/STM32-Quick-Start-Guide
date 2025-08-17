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
	
    HAL_Init();                  // ��ʼ��HAL��    
    Stm32_Clock_Init(336,8,2,7); // ����ʱ��,168Mhz
	delay_init(168);             // ��ʼ����ʱ����
	uart_init(115200);           // ��ʼ��USART
	LED_Init();				     // ��ʼ��LED	
 	LCD_Init();           	     // ��ʼ��LCD
	usmart_dev.init(84); 		 // ��ʼ��USMART	
    RTC_Init();                  // ��ʼ��RTC 
    RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0); // ����WAKE UP�ж�,1�����ж�һ��  
    POINT_COLOR=RED; // ���ñ���ɫΪ��ɫ
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"RTC TEST");	
	LCD_ShowString(30,90,200,16,16,"by hazy1k");
	LCD_ShowString(30,110,200,16,16,"2024/10/11");		   
    while(1)
    {
		t++;
		if((t%10)==0) // ÿ100ms����һ����ʾ����
		{
            HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN); // ��ȡ��ǰʱ��
			sprintf((char*)tbuf,"Time:%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); // ��ӡʱ��
			LCD_ShowString(30,140,210,16,16,tbuf); // ��ʾʱ��
            HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN); // ��ȡ��ǰ����
			sprintf((char*)tbuf,"Date:20%02d-%02d-%02d",RTC_DateStruct.Year,RTC_DateStruct.Month,RTC_DateStruct.Date); // ��ӡ���� 
			LCD_ShowString(30,160,210,16,16,tbuf); // ��ʾ����
			sprintf((char*)tbuf,"Week:%d",RTC_DateStruct.WeekDay); // ��ӡ����
			LCD_ShowString(30,180,210,16,16,tbuf); // ��ʾ����
		} 
		if((t%20)==0)LED0=!LED0; // ÿ200ms,��תһ��LED0 
        delay_ms(10);
	}
}