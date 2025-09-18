#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "wwdg.h"
#include "SysTick.h"
#include "usart.h"
#include <stdio.h>

int main(void) 
{
  SysTick_Init();
  BSP_LED_Init();
  BSP_KEY_Init();
  USARTx_Init();
  LED_ON(RED_LED_Pin);
  Delay_ms(1000);
  WWDG_Init(0x7F, 0x5F, WWDG_Prescaler_8); // 递减计数器初值：0x7F 窗口值：0x5F
  printf("wwdg reset\r\n"); 
  while(1)
  {
    LED_OFF(RED_LED_Pin);
    /*
    这部分应该为WWDG监控的程序代码
    计时器值，初始化成最大0X7F，当开启WWDG时候，这个值会不断减小
    当计数器的值大于窗口值时喂狗的话，会复位，当计数器减少到0X40
    还没有喂狗的话就非常非常危险了，计数器再减一次到了0X3F时就复位
    所以要当计数器的值在窗口值和0X40之间的时候喂狗，其中0X40是固定的
    */
    if(KEY_Scan(KEY1_GPIO_Port, KEY1_GPIO_Pin) == KEY_ON)
    {
      printf("wwdg feed\r\n");
      WWDG_Feed();
    }
  }
}
