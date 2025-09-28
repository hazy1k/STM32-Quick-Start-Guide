#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h" 
#include "adc.h"
#include "key.h"
#include "dac.h"
#include "usmart.h"
//ALIENTEK 探索者STM32F407开发板 实验21
//DAC 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
    
int main(void)
{       
	u16 adcx;
	float temp;
 	u8 t=0;	 
	u16 dacval=0;
	u8 key;	 
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(84,115200);		//初始化串口波特率为115200  	
	usmart_dev.init(84);		//初始化USMART
	LED_Init();					//初始化LED 
 	LCD_Init();					//LCD初始化
	Adc_Init(); 				//adc初始化
	KEY_Init(); 				//按键初始化
	Dac1_Init();		 		//DAC通道1初始化	
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"DAC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/5/7");	 
	LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");	 
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
	LCD_ShowString(30,150,200,16,16,"DAC VAL:");	      
	LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");	      
	LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V");
	DAC->DHR12R1=dacval;		//初始值为0	    		
	while(1)
	{
		t++;
		key=KEY_Scan(0);			  
		if(key==WKUP_PRES)
		{		 
			if(dacval<4000)dacval+=200;
			DAC->DHR12R1=dacval;		//输出	
		}else if(key==KEY1_PRES)	
		{
			if(dacval>200)dacval-=200;
			else dacval=0;
			DAC->DHR12R1=dacval; 		//输出
		}	 
		if(t==10||key==KEY1_PRES||key==WKUP_PRES) 	//WKUP/KEY1按下了,或者定时时间到了
		{	  
 			adcx=DAC->DHR12R1;
			LCD_ShowxNum(94,150,adcx,4,16,0);     	//显示DAC寄存器值
			temp=(float)adcx*(3.3/4096);			//得到DAC电压值
			adcx=temp;
 			LCD_ShowxNum(94,170,temp,1,16,0);     	//显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0X80); 	//显示电压值的小数部分
 			adcx=Get_Adc_Average(ADC_CH5,20);		//得到ADC转换值	  
			temp=(float)adcx*(3.3/4096);			//得到ADC电压值
			adcx=temp;
 			LCD_ShowxNum(94,190,temp,1,16,0);     	//显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,190,temp,3,16,0X80); 	//显示电压值的小数部分
			LED0=!LED0;	   
			t=0;
		}	    
		delay_ms(10);	 
	}	
}

















