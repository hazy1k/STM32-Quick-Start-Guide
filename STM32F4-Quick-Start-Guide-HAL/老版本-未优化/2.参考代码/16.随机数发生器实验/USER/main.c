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
	
    HAL_Init();                 // ��ʼ��HAL��    
    Stm32_Clock_Init(336,8,2,7);// ����ʱ��,168Mhz
	delay_init(168);            // ��ʼ����ʱ����
	uart_init(115200);          // ��ʼ��USART
	LED_Init();				    // ��ʼ��LED	
	KEY_Init();				    // ��ʼ��KEY
 	LCD_Init();           		// ��ʼ��LCD
	usmart_dev.init(84); 		// ��ʼ��USMART	
	POINT_COLOR = RED; // ����������ɫΪ��ɫ	
    while(RNG_Init())	 		    //��ʼ�������������
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
		key = KEY_Scan(0); // ��ⰴ��
		if(key==KEY0_PRES) // ����KEY0������
		{
			random = RNG_Get_RandomNum(); //��������
			LCD_ShowNum(30+8*11,180,random,10,16); //��ʾ�����
		} 
		if((t%20)==0) // ÿ200ms
		{ 
			LED0 = !LED0;	                     // ÿ200ms,��תһ��LED0 
			random = RNG_Get_RandomRange(0,9);   // ��ȡ[0,9]����������
			LCD_ShowNum(30+8*16,210,random,1,16);//��ʾ�����
		 }
		delay_ms(10);
		t++;
	} 
}