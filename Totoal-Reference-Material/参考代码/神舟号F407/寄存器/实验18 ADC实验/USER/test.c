#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h" 
#include "adc.h"
//ALIENTEK 探索者STM32F407开发板 实验18
//ADC 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
    
int main(void)
{      
 	u16 adcx;
	float temp;
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(84,115200);		//初始化串口波特率为115200   
	LED_Init();					//初始化LED 
 	LCD_Init();					//初始化LCD
	Adc_Init(); 				//初始化ADC
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"ADC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/5/6");	  
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(30,130,200,16,16,"ADC1_CH5_VAL:");	      
	LCD_ShowString(30,150,200,16,16,"ADC1_CH5_VOL:0.000V");	      
	while(1)
	{
		adcx=Get_Adc_Average(ADC_CH5,20);
		LCD_ShowxNum(134,130,adcx,4,16,0);//显示ADC的值
		temp=(float)adcx*(3.3/4096); 
		adcx=temp;
		LCD_ShowxNum(134,150,adcx,1,16,0);//显示电压值
		temp-=adcx;
		temp*=1000;
		LCD_ShowxNum(150,150,temp,3,16,0X80);
		LED0=!LED0;
		delay_ms(250);	
	}
}

















