#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"

int main(void)
{
  short temp;
  bsp_init();
  adc_temp_init();
  LCD_ShowString(30,50,200,16,16,"STM32F4 ADC Temperature");
  LCD_ShowString(30,120,200,16,16, "TEMPERATE: 00.00C");
	while(1)
  {
    temp = adc_get_temp();
    if(temp < 0)
    {
      temp = -temp;
      LCD_ShowString(110,120,16,16,16,"-");
    }
    else
    {
      LCD_ShowString(110,120,16,16,16," ");
    }
    LCD_ShowxNum(118,120,temp/100,2,16,0);
    LCD_ShowxNum(142,120,temp%100,2,16,0x80);
    LED_TOGGLE(LED0_GPIO_Pin);
    delay_ms(100);
  }
}
