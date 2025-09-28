#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h" 
#include "adc.h"
#include "key.h"
#include "timer.h"
#include "usmart.h" 
//ALIENTEK 探索者STM32F407开发板 实验22
//PWM DAC 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司

//设置输出电压
//vol:0~330,代表0~3.3V
void PWM_DAC_Set(u16 vol)
{
	double temp=vol;
	temp/=100;
	temp=temp*256/3.3;
	PWM_DAC_VAL=temp;	   
} 

int main(void)
{       
	u16 adcx;
	float temp;
 	u8 t=0;	 
	u16 pwmval=0;
	u8 key; 
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			//延时初始化  
	uart_init(84,115200);		//初始化串口波特率为115200  	
	usmart_dev.init(84);		//初始化USMART
	LED_Init();					//初始化LED 
 	LCD_Init();					//LCD初始化
	Adc_Init(); 				//adc初始化
	KEY_Init(); 				//按键初始化
	TIM9_CH2_PWM_Init(255,1);	//TIM9 PWM初始化, Fpwm=84M/256=328.125Khz.
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"PWM DAC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/5/7");	 
	LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");	 
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
	LCD_ShowString(30,150,200,16,16,"DAC VAL:");	      
	LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");	      
	LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V");
	PWM_DAC_VAL=pwmval;//初始值为0	    	      
	while(1)
	{
		t++;
		key=KEY_Scan(0);			  
		if(key==WKUP_PRES)
		{		 
			if(pwmval<250)pwmval+=10;
			PWM_DAC_VAL=pwmval;		//输出	
		}else if(key==KEY1_PRES)	
		{
			if(pwmval>10)pwmval-=10;
			else pwmval=0;
			PWM_DAC_VAL=pwmval; 		//输出
		}	 
		if(t==10||key==KEY1_PRES||key==WKUP_PRES) 	//WKUP/KEY1按下了,或者定时时间到了
		{	  
 			adcx=PWM_DAC_VAL;
			LCD_ShowxNum(94,150,adcx,3,16,0);     	//显示DAC寄存器值
			temp=(float)adcx*(3.3/256);;			//得到DAC电压值
			adcx=temp;
 			LCD_ShowxNum(94,170,temp,1,16,0);     	//显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0x80); 	//显示电压值的小数部分
 			adcx=Get_Adc_Average(ADC_CH5,20);  		//得到ADC转换值	  
			temp=(float)adcx*(3.3/4096);			//得到ADC电压值
			adcx=temp;
 			LCD_ShowxNum(94,190,temp,1,16,0);     	//显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,190,temp,3,16,0x80); 	//显示电压值的小数部分
			t=0;
			LED0=!LED0;	   
		}	    
		delay_ms(10); 
	}
}



















