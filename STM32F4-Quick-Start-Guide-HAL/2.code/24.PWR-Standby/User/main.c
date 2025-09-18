#include "bsp_init.h"
#include "stdio.h"
#include "pwr.h"

int main(void)
{
  uint8_t i = 0;
  uint8_t key;
  bsp_init();
  pwr_wkup_key_init();
  LCD_ShowString(30,50,200,16,16,"PWR Standby Test");	
	while(1)
  {
    key = key_scan(0);
    if(key == KEY0_Press) // 按下KEY0进入停止模式，按下WKUP键唤醒
    {
      LED_ON(LED1_GPIO_Pin);
      printf("Enter Standby Mode\r\n");
      delay_ms(1000);
      pwr_enter_standby(); // 进入待机模式
      /* 从待机模式唤醒相当于系统重启(复位), 因此不会执行到这里 */
    }
    if((i%20) == 0)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
    }
    delay_ms(10);
    i++;
  }
}
