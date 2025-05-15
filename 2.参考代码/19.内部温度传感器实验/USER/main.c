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
	
    HAL_Init();                  // ��ʼ��HAL��    
    Stm32_Clock_Init(336,8,2,7); // ����ʱ��,168Mhz
	delay_init(168);             // ��ʼ����ʱ����
	uart_init(115200);           // ��ʼ��USART
	usmart_dev.init(84); 		 // ��ʼ��USMART
	LED_Init();				     // ��ʼ��LED	
	KEY_Init();				     // ��ʼ��KEY
 	LCD_Init();           		 // ��ʼ��LCD
    MY_ADC_Init();               // ��ʼ��ADC1
	POINT_COLOR = RED; // ��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"Temperature TEST");  
	POINT_COLOR=BLUE;//��������Ϊ��ɫ      
	LCD_ShowString(30,140,200,16,16,"TEM PERATE: 00.00C");//���ڹ̶�λ����ʾС����	
    while(1)
	{
		temp = Get_Temprate(); // �õ��¶�ֵ 
		if(temp < 0)
		{
			temp =- temp; // ��ʾ����
			LCD_ShowString(30+10*8,140,16,16,16,"-");	// ��ʾ����
		}else LCD_ShowString(30+10*8,140,16,16,16," ");	// �޷���
		LCD_ShowxNum(30+11*8,140,temp/100,2,16,0);		//��ʾ��������
		LCD_ShowxNum(30+14*8,140,temp%100,2,16,0);		//��ʾС������ 
		LED0 = !LED0;
		delay_ms(250);	
	} 
}
