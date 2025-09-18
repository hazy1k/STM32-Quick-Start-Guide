#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"
#include "dac.h"
#include "math.h"

uint16_t dac_sin_buf[4096];

/**
 * @brief       产生正弦波函序列
 *   @note      需保证 : maxval > samples/2
 * @param       maxval : 最大值(0 < maxval < 2048)
 * @param       samples: 采样点的个数
 * @retval      无
 */
void dac_creat_sin_buf(uint16_t maxval, uint16_t samples)
{
    uint8_t i;
    float inc = (2 * 3.1415962) / samples; // 计算增量（一个周期DAC_SIN_BUF个点）
    float outdata = 0;
    for (i = 0; i < samples; i++)
    {
        outdata = maxval * (1 + sin(inc * i)); // 计算以dots个点为周期的每个点的值，放大maxval倍，并偏移到正数区域
        if (outdata > 4095)
            outdata = 4095;                    
        //printf("%f\r\n", outdata);
        dac_sin_buf[i] = outdata;
    }
}

// 设置正弦波输出参数
void dac_dma_sin_set(uint16_t arr, uint16_t psc)
{
  dac_dma_wave_enable(1,100,arr,psc);
}

int main(void)
{
  uint8_t i = 0;
  uint8_t key_value = 0;
  float temp = 0;
  uint16_t adc_value = 0;
  bsp_init();
  adc_init();
  dac_dma_init(1);
  LCD_ShowString(30,50,200,16,16,"STM32F4 DAC SineWave");
  LCD_ShowString(30,110,200,16,16,"KEY0:3KHZ  KEY1:30KHZ");
  LCD_ShowString(30,130,200,16,16,"DAC VAL:");
  LCD_ShowString(30,150,200,16,16,"DAC VOL:0.000V");
  LCD_ShowString(30,170,200,16,16,"ADC VAL:0.000V");
  dac_creat_sin_buf(2048,100);
  dac_dma_wave_enable(1,100,10-1,84-1);
	while(1)
  {
    i++;
    key_value = key_scan(0);
    if(key_value == KEY0_Press)
    {
      dac_creat_sin_buf(2048,100);
      dac_dma_wave_enable(1,100,10-1,28-1);/* 300Khz触发频率, 100个点, 可以得到最高3KHz的正弦波. */
    }
    else if(key_value == KEY1_Press)
    {
      dac_creat_sin_buf(2048,10);
      dac_dma_wave_enable(1,10,10-1,28-1);/* 300Khz触发频率, 10个点, 可以得到最高30KHz的正弦波. */
    }
    // 显示DAC输出值
    adc_value = DAC1->DHR12R1;  
    LCD_ShowxNum(94,130,adc_value,4,16,0);
    temp = (float)adc_value*(3.3/4095);
    adc_value = temp;
    LCD_ShowxNum(94,150,temp,1,16,0);
    temp -= adc_value;
    temp *= 1000;
    LCD_ShowxNum(110,150,temp,3,16,0x80);
    // 显示ADC输入值
    adc_value = adc_get_result_average(ADC_CHANNEL_5 ,20);
    temp = (float)adc_value*(3.3/4095);
    adc_value = temp;
    LCD_ShowxNum(94,170,temp,1,16,0);
    temp -= adc_value;
    temp *= 1000;
    LCD_ShowxNum(110,170,temp,3,16,0x80);
    if(i == 10)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
      i = 0;
    }
    delay_ms(5);
  }
}
