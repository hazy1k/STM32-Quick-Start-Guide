#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "iwdg.h"
#include "usart.h"
#include "stdio.h"

int main(void)
{
  BSP_LED_Init();
  BSP_KEY_Init();
  USARTx_Init();
  IWDG_Init(IWDG_Prescaler_64, 625); // 大约1s
  if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  {
    printf("IWDG Reset\r\n");
    // 独立看门狗复位
    LED_ON(RED_LED_Pin);
    RCC_ClearFlag();
  }
  else
  {
    printf("Other Reset\r\n");
    // 其他复位
    LED_ON(BLUE_LED_Pin);
  }
  while(1)
  {
    if(KEY_Scan(KEY1_GPIO_Port, KEY1_GPIO_Pin) == KEY_ON)
    {
      printf("IWDG Feed\r\n");
      IWDG_Feed();
      LED_ON(GREEN_LED_Pin);
    }
  }
}
