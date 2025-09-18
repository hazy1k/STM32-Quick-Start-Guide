#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"
#include "dac.h"

extern TIM_HandleTypeDef tim_handle;

int main(void)
{
  uint16_t adc_value = 0;
  float temp;
  uint8_t i = 0;
  uint8_t key_value = 0;
  uint16_t dac_value;
  bsp_init();
  adc_init();
  pwmdac_init(256-1,0); // PWM DAC初始化,Fpwm = 84 / 256 =328.125Khz

  LCD_ShowString(30,130,200,16,16,"PWM VAL:");
  LCD_ShowString(30,150,200,16,16,"DAC VOL:0.000V");
  LCD_ShowString(30,170,200,16,16,"ADC VAL:0.000V");

  __HAL_TIM_SET_COMPARE(&tim_handle, TIM_CHANNEL_2, dac_value);
  while(1)
  {
    i++;
    key_value = key_scan(0);
    if(key_value == WKUP_Press)
    {
      if(dac_value < 250)
      {
        dac_value += 10;
      }
      __HAL_TIM_SET_COMPARE(&tim_handle, TIM_CHANNEL_2, dac_value);
    }
    else if(key_value == KEY1_Press)
    {
      if(dac_value > 10)
      {
        dac_value -= 10;
      }
      else
      {
        dac_value = 0;
      }
      __HAL_TIM_SET_COMPARE(&tim_handle, TIM_CHANNEL_2, dac_value);
    }

    if(i == 10||key_value == WKUP_Press||key_value == KEY1_Press)
    {
      /* DAC值读取计算 */
      adc_value = __HAL_TIM_GET_COMPARE(&tim_handle, TIM_CHANNEL_2);
      LCD_ShowxNum(94,130,adc_value,3,16,0);
      temp = (float)adc_value*(3.3/256);
      adc_value = temp;
      LCD_ShowxNum(94,150,temp,1,16,0);
      temp -= adc_value;
      temp *= 1000;
      LCD_ShowxNum(110,150,temp,3,16,0x80);
      /* ADC值读取计算 */
      adc_value = adc_get_result_average(ADC_CHANNEL_5, 10);
      temp = (float)adc_value*(3.3/4096);
      adc_value = temp;
      LCD_ShowxNum(94,170,temp,1,16,0);
      temp -= adc_value;
      temp *= 1000;
      LCD_ShowxNum(110,170,temp,3,16,0x80);
      
      LED_TOGGLE(LED0_GPIO_Pin);
      i = 0;
    }
    delay_ms(5);
  }
}
