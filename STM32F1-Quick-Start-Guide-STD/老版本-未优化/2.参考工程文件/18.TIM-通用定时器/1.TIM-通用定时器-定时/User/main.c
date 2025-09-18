// 通用定时器TIMx,x[2,3,4,5]定时应用
#include "stm32f10x.h"
#include "led.h"
#include "GENERALTim.h" 
#include "usart.h"

uint32_t time = 0;

int main(void)
{
  LED_Init();
  USART_Config();
  TIMx_Init();
  while(1)
  {
    if(time == 1000) // 1000*1 = 1s
    {
      time = 0;
      printf("这是一个由TIM2产生中断发出的句子哦\r\n");
      LED1_TOGGLE();
    }

  }
}
