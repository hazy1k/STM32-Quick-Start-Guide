/* 该文件统一用于存放中断服务函数 */
#include "stm32f10x_it.h"

#include "board.h" 


extern __IO uint16_t ADC_ConvertedValue;

void ADC_IRQHandler(void)
{	
	if (ADC_GetITStatus(ADCx,ADC_IT_EOC)==SET) 
	{
		// 读取ADC的转换值
		ADC_ConvertedValue = ADC_GetConversionValue(ADCx);
	}
	ADC_ClearITPendingBit(ADCx,ADC_IT_EOC);
}




