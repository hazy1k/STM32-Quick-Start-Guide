#include "bsp_init.h"
#include "stdio.h"
#include "pwr.h"

int main(void)
{
  uint8_t i = 0;
  uint8_t key;
  bsp_init();
  pwr_wkup_key_init();
  LCD_ShowString(30,50,200,16,16,"PWR Stop Test");	
	while(1)
  {
    key = key_scan(0);
    if(key == KEY0_Press) // 按下KEY0进入停止模式，按下WKUP键唤醒
    {
      LED_ON(LED1_GPIO_Pin);
      printf("Enter Stop Mode\r\n");
      delay_ms(1000);
      pwr_enter_stop(); // 进入停止模式

      /*从停止模式唤醒，需要重新配置时钟*/
      sys_stm32_clock_init(336,8,2,7);
      HAL_ResumeTick();
      LED_OFF(LED1_GPIO_Pin);
    }
    if((i%20) == 0)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
    }
    delay_ms(10);
    i++;
  }
}
