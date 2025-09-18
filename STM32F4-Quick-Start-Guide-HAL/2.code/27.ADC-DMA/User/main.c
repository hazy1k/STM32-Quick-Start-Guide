#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"

uint16_t adc_dma_buf[ADC_DMA_BUFFSIZE]; // DMA缓冲区
extern uint8_t adc_dma_sta; // DMA传输完成标志

int main(void)
{
  uint16_t i;
  uint32_t sum;
  float voltage_val;

  bsp_init(); 

  // 初始化ADC DMA采集。
  // 在此函数内部，DMA已经被设置为CIRCULAR模式并启动。
  adc_dma_init((uint32_t)adc_dma_buf);

  LCD_ShowString(30, 50, 200, 16, 16, "STM32F4 ADC DMA Test");
  LCD_ShowString(30, 110, 200, 16, 16, "ADC1_CH5_VAL:");
  LCD_ShowString(30, 130, 200, 16, 16, "ADC1_CH5_VOL:0.000V");

  // 由于 adc_dma_init 已经通过 HAL_ADC_Start_DMA 启动了DMA和ADC
  // 并且DMA是CIRCULAR模式，这里不需要再调用 adc_dma_enable。
  // 如果您在 adc_dma_init 中没有调用 HAL_ADC_Start_DMA，那么在这里调用一次是必需的。
  // 目前的 adc_dma_init 已经包含了 HAL_ADC_Start_DMA，所以这行可以注释掉或移除。
  // adc_dma_enable(ADC_DMA_BUFFSIZE); // 此行在此方案中不再需要

  while(1)
  {
      // 检查DMA传输是否完成一个缓冲区的数据
      if(adc_dma_sta)
      {
          sum = 0;
          // 遍历DMA缓冲区，计算平均值
          for(i = 0; i < ADC_DMA_BUFFSIZE; i++)
          {
              sum += adc_dma_buf[i];
          }
          // 计算平均ADC值
          uint16_t avg_adc_value = sum / ADC_DMA_BUFFSIZE;
          LCD_ShowxNum(134, 110, avg_adc_value, 4, 16, 0); // 显示平均ADC值

          // 计算电压值 (3.3V 参考电压，12位ADC，最大值为4095)
          voltage_val = (float)avg_adc_value * (3.3f / 4095.0f); // 12位ADC最大值为4095

          // 显示电压整数部分
          uint16_t voltage_int = (uint16_t)voltage_val;
          LCD_ShowxNum(134, 130, voltage_int, 1, 16, 0);

          // 显示电压小数部分 (取小数点后三位)
          uint16_t voltage_frac = (uint16_t)((voltage_val - voltage_int) * 1000);
          LCD_ShowxNum(150, 130, voltage_frac, 3, 16, 0x80);
          adc_dma_sta = 0; // 清除完成标志，等待下一次DMA传输完成回调
      }
      LED_TOGGLE(LED0_GPIO_Pin);
      delay_ms(100);
  }
}
