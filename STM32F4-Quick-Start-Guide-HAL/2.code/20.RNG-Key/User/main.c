#include "bsp_init.h"
#include "stdio.h"
#include "usmart.h"
#include "rng.h"

int main(void)
{
  uint32_t random_number;
  uint8_t i = 0;
  uint8_t key;
  bsp_init();
  usmart_dev.init(84);
  LCD_ShowString(30,50,200,16,16,"RNG Test");
  while(RNG_Init())
  {
    LCD_ShowString(30,110,200,16,16,"RNG Init Error");
    delay_ms(1000);
    LCD_ShowString(30,110,200,16,16,"RNG Trying...");
  }
  LCD_ShowString(30,110,200,16,16,"RNG Init Success");
	while(1)
  {
    key = key_scan(0);
    if(key == KEY0_Press)
    {
      random_number = rng_get_random();
      LCD_ShowNum(128, 150, random_number, 10, 16);
    }
    if((i % 20) == 0)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
      random_number = rng_get_random_range(0,100); // 获取100以内的随机数
      LCD_ShowNum(128, 170, random_number, 10, 16);
    }
    i++;
    delay_ms(10);
  }
}
