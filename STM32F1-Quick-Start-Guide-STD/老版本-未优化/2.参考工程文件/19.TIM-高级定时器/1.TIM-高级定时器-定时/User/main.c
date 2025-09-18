#include "stm32f10x.h"
#include "led.h"
#include "AdvanceTim.h" 
#include "usart.h"

extern volatile uint32_t time;

int main(void)
{ 
	LED_Init();
	ATIM_Config();
	USART_Config();
  while(1)
  {
    if(time == 1000) // 1000 * 1 ms = 1s 时间到 
    {
      time = 0;
			LED1_TOGGLE(); 
			printf("这是由TIM1产生的1s定时器中断\r\n");
    }        
  }
}
