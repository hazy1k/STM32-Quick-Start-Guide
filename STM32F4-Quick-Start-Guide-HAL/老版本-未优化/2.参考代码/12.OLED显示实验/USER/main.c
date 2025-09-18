#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "oled.h"

int main(void)
{
	u8 t = 0; 
	
    HAL_Init();                   	//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
	delay_init(168);               	//初始化延时函数
	uart_init(115200);             	//初始化USART
	LED_Init();						//初始化LED	
	OLED_Init();					//初始化OLED	
	OLED_ShowString(0,0,"ALIENTEK",24);  // 函数参数：起始行，起始列，显示字符串，字体大小，
	OLED_ShowString(0,24, "0.96' OLED TEST",16);  
 	OLED_ShowString(0,40,"ATOM 2024/10/7",12);  
 	OLED_ShowString(0,52,"ASCII:",12);  
 	OLED_ShowString(64,52,"CODE:",12);  
	OLED_Refresh_Gram(); // 更新显示到OLED	 
	t=' ';  
	
   	while(1)
	{					  						  		 
		OLED_ShowChar(36,52,t,12,1); // 显示ASCII字符，参数：起始列，起始行，ASCII码，字体大小，显示模式
		OLED_ShowNum(94,52,t,3,12);	 // 显示ASCII字符的码值，参数：起始列，起始行，数字，数字长度，字体大小 
		OLED_Refresh_Gram();         // 更新显示到OLED
		t++;
		if(t>'~')t=' ';  
		delay_ms(500);
		LED0=!LED0;;
	}
}