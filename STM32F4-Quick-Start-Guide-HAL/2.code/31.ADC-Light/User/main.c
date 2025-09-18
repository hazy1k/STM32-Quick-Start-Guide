#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"

int main(void)
{
  uint16_t temp;
  bsp_init();
  light_sensor_init();
  LCD_ShowString(30,50,200,16,16,"LIGHT SENSOR TEST");
  LCD_ShowString(30,110,200,16,16, "LIGHT_VAL:");
	while(1)
  {
    temp = light_sensor_read();
    LCD_ShowxNum(110,110,temp,3,16,0);
    LED_TOGGLE(LED0_GPIO_Pin);
    delay_ms(100);
  }
}
