#include "stm32f10x.h"
#include "usart.h"
#include "ADC.h"
#include "SysTick.h"

extern __IO uint16_t ADC_ConvertedValue;
float ADC_Result;

int main(void)
{
    SysTick_Init();
    USART_Config();
    ADCx_Init();
    while (1)
    {
        // 检查ADC是否完成转换
        if(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC))
        {
            // 如果DMA启用，直接从内存变量获取值
            ADC_ConvertedValue = ADC_GetConversionValue(ADCx); // 读取转换结果
            ADC_ClearFlag(ADCx, ADC_FLAG_EOC);  // 清除标志位
            ADC_Result = (float)ADC_ConvertedValue / 4096 * 3.3;
            printf("\r\n ADC采集到的值 = 0x%04X \r\n", ADC_ConvertedValue);
            printf("\r\n 计算得到的电压值 = %f V \r\n", ADC_Result);
			Delay_ms(1000); 
        }
        Delay_ms(2000);
    }
}
