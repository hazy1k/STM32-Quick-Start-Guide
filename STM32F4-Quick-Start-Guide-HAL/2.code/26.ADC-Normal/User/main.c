#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"

int main(void)
{
  uint16_t adc_value;
  float temp;
  bsp_init();
  adc_init();
  LCD_ShowString(30,50,200,16,16,"STM32F4 ADC Test");
  LCD_ShowString(30,110,200,16,16, "ADC1_CH5_VAL:");
  LCD_ShowString(30,130,200,16,16, "ADC1_CH5_VOL:0.000V");
	while(1)
  {
    adc_value = adc_get_result_average(ADC_CHANNEL_5, 10); // 10次采样取平均值
    LCD_ShowxNum(134,110,adc_value,5,16,0);
    temp = (float)adc_value*(3.3/4096); // 计算电压值
    adc_value = temp;
    LCD_ShowxNum(134,130,adc_value,1,16,0); // 显示电压整数部分
    temp -= adc_value;
    temp *= 1000; // 计算电压小数部分
    LCD_ShowxNum(150,130,temp,3,16,0x80);
    LED_TOGGLE(LED0_GPIO_Pin);
    delay_ms(100);
  }
}
