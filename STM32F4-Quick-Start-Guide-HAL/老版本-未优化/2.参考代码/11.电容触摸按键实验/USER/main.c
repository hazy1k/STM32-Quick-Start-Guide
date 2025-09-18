#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "tpad.h"

int main(void)
{
	u8 t=0; 
    HAL_Init();                   	
    Stm32_Clock_Init(336,8,2,7);  	
	delay_init(168);               	
	uart_init(115200);            
	LED_Init();						
	tpad_init(8); 
   	while(1)
	{					  						  		 
 		if(TPAD_Scan(0)) // 成功捕获到了一次上升沿(此函数执行时间至少15ms)
		{
			LED1=!LED1;	// LED1取反
		}
		t++;
		if(t == 15)		 
		{
			t = 0;
			LED0 = !LED0; // LED0取反,提示程序正在运行
		}
		delay_ms(10);
	}
}
