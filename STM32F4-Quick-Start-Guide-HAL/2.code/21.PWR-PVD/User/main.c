#include "bsp_init.h"
#include "stdio.h"
#include "pvd.h"

int main(void)
{
  uint8_t i = 0;
  bsp_init();
  LCD_ShowString(30,50,200,16,16,"PVD Test");	
  pwr_pvd_init(PWR_PVDLEVEL_7); // PVD 2.9V检  
	while(1)
  {
    if((i%20) == 0)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
    }
    delay_ms(10);
    i++;
  }
}
