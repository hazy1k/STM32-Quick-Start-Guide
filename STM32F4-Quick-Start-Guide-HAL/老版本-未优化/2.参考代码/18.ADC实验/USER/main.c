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
    u16 adcx;
	float temp;
    HAL_Init();                  // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7); // 设置时钟,168Mhz
	delay_init(168);             // 初始化延时函数
	uart_init(115200);           // 初始化USART
	usmart_dev.init(84); 		 // 初始化USMART
	LED_Init();				 	 // 初始化LED	
	KEY_Init();					 // 初始化KEY
 	LCD_Init();           		 // 初始化LCD
    MY_ADC_Init();               // 初始化ADC1通道5
	POINT_COLOR = RED; // 设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"ADC TEST");	  
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(30,130,200,16,16,"ADC1_CH5_VAL:"); // 显示提示信息	      
	LCD_ShowString(30,150,200,16,16,"ADC1_CH5_VOL:0.000V");	// 先在固定位置显示小数点  	
    while(1)
	{
        adcx = Get_Adc_Average(ADC_CHANNEL_5,20); // 获取通道5的转换值，20次取平均
		LCD_ShowxNum(134,130,adcx,4,16,0);        // 显示ADCC采样后的原始值
		temp=(float)adcx*(3.3/4096);              // 获取计算后的带小数的实际电压值，比如3.1111
		adcx=temp;                                // 赋值整数部分给adcx变量，因为adcx为u16整形
		LCD_ShowxNum(134,150,adcx,1,16,0);        // 显示电压值的整数部分，3.1111的话，这里就是显示3
		temp-=adcx;                               // 把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111
		temp*=1000;                               // 小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。
		LCD_ShowxNum(150,150,temp,3,16,0X80);     // 显示小数部分（前面转换为了整形显示），这里显示的就是111.
		LED0 = !LED0;
		delay_ms(250);	
	} 
}
