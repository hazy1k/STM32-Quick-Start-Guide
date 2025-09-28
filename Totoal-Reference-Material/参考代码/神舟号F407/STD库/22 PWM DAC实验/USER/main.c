#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "adc.h"
#include "key.h"
#include "pwmdac.h"
 
 
//ALIENTEK 探索者STM32F407开发板 实验22
//PWM DAC 实验  -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK
 

  
int main(void)
{ 
	u16 adcx;
	float temp;
 	u8 t=0;	 
	u16 pwmval=0;
	u8 key; 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);    //初始化延时函数
	uart_init(115200);	//初始化串口波特率为115200
	
	LED_Init();					//初始化LED 
 	LCD_Init();					//LCD初始化
	Adc_Init(); 				//adc初始化
	KEY_Init(); 				//按键初始化
	TIM9_CH2_PWM_Init(255,0);	//TIM4 PWM初始化, Fpwm=168M/256=656.25Khz.
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"PWM DAC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/5/6");	 
	LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");	 
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
	LCD_ShowString(30,150,200,16,16,"DAC VAL:");	      
	LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");	      
	LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V");
  TIM_SetCompare2(TIM9,pwmval);	//初始值     	      
	while(1)
	{
		t++;
		key=KEY_Scan(0);			  
		if(key==4)
		{		 
			if(pwmval<250)pwmval+=10;
			TIM_SetCompare2(TIM9,pwmval);	//输出	
		}else if(key==2)	
		{
			if(pwmval>10)pwmval-=10;
			else pwmval=0;
			TIM_SetCompare2(TIM9,pwmval);	//输出
		}	 
		if(t==10||key==2||key==4) 		//WKUP/KEY1按下了,或者定时时间到了
		{	  
 			adcx=TIM_GetCapture2(TIM9);;
			LCD_ShowxNum(94,150,adcx,3,16,0);     	//显示DAC寄存器值
			temp=(float)adcx*(3.3/256);;			      //得到DAC电压值
			adcx=temp;
 			LCD_ShowxNum(94,170,temp,1,16,0);     	//显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0x80); 	//显示电压值的小数部分
 			adcx=Get_Adc_Average(ADC_Channel_5,20); //得到ADC转换值	  
			temp=(float)adcx*(3.3/4096);			      //得到ADC电压值
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




