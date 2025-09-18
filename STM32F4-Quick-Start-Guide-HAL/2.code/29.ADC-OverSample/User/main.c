#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"

extern uint8_t adc_dma_sta; // DMA传输完成标志
extern ADC_HandleTypeDef adc_handle;
/* ADC过采样技术, 是利用ADC多次采集的方式, 来提高ADC精度, 采样速度每提高4倍
 * 采样精度提高 1bit, 同时, ADC采样速度降低4倍, 如提高4bit精度, 需要256次采集
 * 才能得出1次数据, 相当于ADC速度慢了256倍. 理论上只要ADC足够快, 我们可以无限
 * 提高ADC精度, 但实际上ADC并不是无限快的, 而且由于ADC性能限制, 并不是位数无限
 * 提高结果就越好, 需要根据自己的实际需求和ADC的实际性能来权衡.
 */
#define ADC_OVERSAMPLE_TIMES 256 // 过采样次数
#define ADC_DMA_BUF_SZIE ADC_OVERSAMPLE_TIMES*10 // DMA缓冲区大小
uint16_t ADC_DMA_BUF[ADC_DMA_BUF_SZIE];

int main(void)
{
  uint16_t i;
  uint32_t adc_value,sum;
  float temp;
  bsp_init(); 

  // 初始化ADC DMA采集。
  // 在此函数内部，DMA已经被设置为CIRCULAR模式并启动。
  adc_dma_init((uint32_t)&ADC_DMA_BUF);
  LCD_ShowString(30, 50, 200, 16, 16, "STM32F4 ADC DMA Test");
  LCD_ShowString(30, 110, 200, 16, 16, "ADC1_CH5_VAL:");
  LCD_ShowString(30, 130, 200, 16, 16, "ADC1_CH5_VOL:0.000V");

  // 由于 adc_dma_init 已经通过 HAL_ADC_Start_DMA 启动了DMA和ADC
  // 并且DMA是CIRCULAR模式，这里不需要再调用 adc_dma_enable。
  // 如果您在 adc_dma_init 中没有调用 HAL_ADC_Start_DMA，那么在这里调用一次是必需的。
  // 目前的 adc_dma_init 已经包含了 HAL_ADC_Start_DMA，所以这行可以注释掉或移除。
  adc_dma_enable(ADC_DMA_BUF_SZIE); // 此行在此方案中不再需要
  while(1)
  {
    if(adc_dma_sta)
    {
        sum=0;
        for(i=0;i<ADC_DMA_BUF_SZIE;i++)
        {
            sum += ADC_DMA_BUF[i];
        }
        adc_value = sum/(ADC_DMA_BUF_SZIE/ADC_OVERSAMPLE_TIMES); // 计算平均值
        adc_value >>= 4; // 除以2^4倍, 得到12+4位 ADC精度值, 注意: 提高 N bit精度, 需要 >> N
        LCD_ShowxNum(134,110,adc_value,5,16,0);
        temp = (float)adc_value*(3.3/65536);
        adc_value = temp;
        LCD_ShowxNum(134,130,adc_value,1,16,0);
        temp -= adc_value;
        temp *= 1000;
        LCD_ShowxNum(150,130,temp,3,16,0x80);
        adc_dma_sta = 0;
        adc_dma_enable(ADC_DMA_BUF_SZIE);
    }
    LED_TOGGLE(LED0_GPIO_Pin);
    delay_ms(100);
  }
}
