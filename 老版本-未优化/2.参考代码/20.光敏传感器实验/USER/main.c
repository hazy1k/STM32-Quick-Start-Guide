#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "adc3.h"
#include "lsens.h"

int main(void)
{
	u8 adcx;	
    HAL_Init();                  // ��ʼ��HAL��    
    Stm32_Clock_Init(336,8,2,7); // ����ʱ��,168Mhz
	delay_init(168);             // ��ʼ����ʱ����
	uart_init(115200);           // ��ʼ��USART
	usmart_dev.init(84); 		 // ��ʼ��USMART
	LED_Init();				     // ��ʼ��LED	
	KEY_Init();				     // ��ʼ��KEY
 	LCD_Init();           		 // ��ʼ��LCD
	Lsens_Init(); 				 // ��ʼ������������
	POINT_COLOR = RED; // ��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"LSENS TEST");	  
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(30,130,200,16,16,"LSENS_VAL:");	             
	while(1)
	{
		adcx = Lsens_Get_Val(); // ��ȡ������������ֵ
		LCD_ShowxNum(30+10*8,130,adcx,3,16,0);//��ʾADC��ֵ 
		LED0=!LED0;
		delay_ms(250);	
	}
}
