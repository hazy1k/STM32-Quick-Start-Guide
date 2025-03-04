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
 		if(TPAD_Scan(0)) // �ɹ�������һ��������(�˺���ִ��ʱ������15ms)
		{
			LED1=!LED1;	// LED1ȡ��
		}
		t++;
		if(t == 15)		 
		{
			t = 0;
			LED0 = !LED0; // LED0ȡ��,��ʾ������������
		}
		delay_ms(10);
	}
}
