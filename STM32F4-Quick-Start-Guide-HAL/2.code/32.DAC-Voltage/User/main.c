#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"
#include "dac.h"

extern DAC_HandleTypeDef dac_handle;

int main(void)
{
  uint16_t adc_value;
  float temp;
  uint8_t i =0;
  uint16_t dac_value = 0;
  uint8_t key_value = 0;

  bsp_init();
  adc_init();
  dac_init(1); // 初始化DAC1_OUT1通道 1:DAC_OUT_1-PA4  2:DAC_OUT_2-PA5 
  LCD_ShowString(30,50,200,16,16,"STM32F4 DAC Test");
  LCD_ShowString(30,110,200,16,16,"WK_UP:+  KEY1:-");
  LCD_ShowString(30,130,200,16,16,"DAC VAL:");
  LCD_ShowString(30,150,200,16,16,"DAC VOL:0.000V");
  LCD_ShowString(30,170,200,16,16,"ADC VOL:0.000V");
	while(1)
  {
    i++;
    key_value = key_scan(0);
    if(key_value == WKUP_Press)
    {
      if(dac_value < 4095)
      {
        dac_value += 200;
      }
      HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
    }
    else if(key_value == KEY1_Press)
    {
      if(dac_value > 0)
      {
        dac_value -= 200;
      }
      else
      {
        dac_value = 0;
      }
      HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
    }
    if(i == 0 || key_value == KEY1_Press || key_value == WKUP_Press) // WKUP/KEY1按下了,或者定时时间到了
    {
      adc_value = HAL_DAC_GetValue(&dac_handle, DAC_CHANNEL_1); // 获取DAC输出值
      LCD_ShowxNum(94,130,adc_value,4,16,0);
      temp = (float)adc_value*(3.3/4095); // 计算ADC电压值
      adc_value = temp;
      LCD_ShowxNum(94,150,temp,1,16,0); // 显示电压整数值
      temp -= adc_value;
      temp *= 1000;
      LCD_ShowxNum(110,150,temp,3,16,0x80);
      adc_value = adc_get_result_average(ADC_CHANNEL_5, 20); // 获取ADC电压值
      temp = (float)adc_value*(3.3/4095); // 计算ADC电压值
      adc_value = temp;
      LCD_ShowxNum(94,170,temp,1,16,0); // 显示电压整数值
      temp -= adc_value;
      temp *= 1000;
      LCD_ShowxNum(110,170,temp,3,16,0x80);
      LED_TOGGLE(LED0_GPIO_Pin);
      i = 0;
    }
    delay_ms(10);
  }
}
