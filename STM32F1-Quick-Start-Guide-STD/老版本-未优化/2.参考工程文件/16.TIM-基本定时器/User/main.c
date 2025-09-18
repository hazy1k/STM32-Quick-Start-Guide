#include "stm32f10x.h"
#include "led.h"
#include "TIMbase.h"

volatile uint32_t time = 0; // ms 计时变量 

int main(void)
{
	LED_Init();	
  TIM_BASE_Init();
  while(1)
  {
    if(time == 1000) // 1000 * 1 ms = 1s 
    {
      time = 0;
			LED1_TOGGLE(); 
    }        
  }
}
