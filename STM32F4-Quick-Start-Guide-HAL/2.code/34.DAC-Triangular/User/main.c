#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"
#include "dac.h"

extern DAC_HandleTypeDef dac_handle;

int main(void)
{
  uint8_t i = 0;
  uint8_t key_value = 0;
  bsp_init();
  adc_init();
  dac_init(1); // 初始化DAC1_OUT1通道 1:DAC_OUT_1-PA4  2:DAC_OUT_2-PA5 
  LCD_ShowString(30,50,200,16,16,"STM32F4 DAC Triangular Test");
  LCD_ShowString(30,110,200,16,16,"KEY0:Wave1  WKUP:Wave2");
  LCD_ShowString(30,130,200,16,16,"DAC None");
	while(1)
  {
    i++;
    key_value = key_scan(0);
    if(key_value == KEY0_Press)
    {
      LCD_ShowString(30,130,200,16,16,"DAC Wave1");
      dac_triangular_wave(4095,5,2000,100); /* 幅值4095, 采样点间隔5us, 2000个采样点, 100个波形 */
      LCD_ShowString(30,130,200,16,16,"DAC None ");
    }
    else if(key_value == WKUP_Press)
    {
      LCD_ShowString(30,130,200,16,16,"DAC Wave2");
      dac_triangular_wave(4095,500,20,100); /* 幅值4095, 采样点间隔500us, 20个采样点, 100个波形 */
      LCD_ShowString(30,130,200,16,16,"DAC None ");
    }
    if(i == 10)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
      i = 0;
    }
    delay_ms(10);
  }
}
