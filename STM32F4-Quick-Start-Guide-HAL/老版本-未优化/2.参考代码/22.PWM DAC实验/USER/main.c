#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "adc.h"
#include "pwmdac.h"

// 设置输出电压
// vol:0~330,代表0~3.3V
void PWM_DAC_Set(u16 vol)
{
	double temp=vol;
	temp/=100;
	temp=temp*256/3.3;
	TIM_SetTIM9Compare2(temp);	   
}

int main(void)
{
	u16 adcx;
	float temp;
	u8 t=0;	 
	u16 pwmval=0;
	u8 key;
	
    HAL_Init();                 // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);// 设置时钟,168Mhz
	delay_init(168);            // 初始化延时函数
	uart_init(115200);          // 初始化USART
	usmart_dev.init(84); 		// 初始化USMART
	LED_Init();				 	// 初始化LED	
	KEY_Init();					// 初始化KEY
 	LCD_Init();           		// 初始化LCD
	MY_ADC_Init();              // 初始化ADC1
    TIM9_CH2_PWM_Init(255,1);   // TIM9 PWM初始化, Fpwm=84M/256=328.125Khz.
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"PWM DAC TEST");	 
	LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");	  
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
	LCD_ShowString(30,150,200,16,16,"DAC VAL:");	      
	LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");	      
	LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V"); 	
	TIM_SetTIM9Compare2(pwmval);	// 初始值为0 	
    while(1)
	{
		t++;
		key=KEY_Scan(0);			  
		if(key==WKUP_PRES) // WKUP按下
		{		 
			if(pwmval<250)pwmval+=10;   // 增加10
            TIM_SetTIM9Compare2(pwmval);// 输出		
		}else if(key==KEY1_PRES) // KEY1按下
		{
			if(pwmval>10)pwmval-=10; // 减少10
			else pwmval=0;
            TIM_SetTIM9Compare2(pwmval); // 输出	
		}	 
		if(t==10||key==KEY1_PRES||key==WKUP_PRES) // WKUP/KEY1按下了,或者定时时间到了
		{	  
			adcx=HAL_TIM_ReadCapturedValue(&TIM9_Handler,TIM_CHANNEL_2);
			LCD_ShowxNum(94,150,adcx,3,16,0);  // 显示DAC寄存器值
			temp=(float)adcx*(3.3/256);;	   // 得到DAC电压值
			adcx=temp;
			LCD_ShowxNum(94,170,temp,1,16,0);  // 显示电压值整数部分
			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0x80); 	// 显示电压值的小数部分
			adcx=Get_Adc_Average(ADC_CHANNEL_5,20); // 得到ADC转换值	  
			temp=(float)adcx*(3.3/4096);			// 得到ADC电压值
			adcx=temp;
			LCD_ShowxNum(94,190,temp,1,16,0);     	// 显示电压值整数部分
			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,190,temp,3,16,0x80); 	// 显示电压值的小数部分
			t=0;
			LED0=!LED0;	   
		 }	    
		delay_ms(10); 
	}
}
