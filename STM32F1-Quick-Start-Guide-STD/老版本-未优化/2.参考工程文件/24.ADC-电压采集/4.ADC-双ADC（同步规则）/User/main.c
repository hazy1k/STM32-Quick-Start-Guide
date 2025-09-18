#include "stm32f10x.h"
#include "ADC.h"
#include "SysTick.h"
#include "usart.h"
// 双模式时，ADC1和ADC2转换的数据都存放在ADC1的数据寄存器，
// ADC1的在低十六位，ADC2的在高十六位
extern __IO uint32_t ADC_ConvertedValue[1];
float ADC_Result[2];
int main(void)
{
	ADCx_Init();
	USART_Config();
	SysTick_Init();
	uint16_t temp_h = 0, temp_l = 0;
	while(1)
	{
		temp_h = (ADC_ConvertedValue[0]&0xFFFF0000) >> 16;
		temp_l = (ADC_ConvertedValue[0]&0xFFFF);
		ADC_Result[0] = (float)temp_h/4096*3.3;
		ADC_Result[1] = (float)temp_l/4096*3.3;
		printf("ADC_1 of Vref = %f V\r\n", ADC_Result[0]);
		printf("ADC_2 of Vref = %f V\r\n", ADC_Result[1]);
		ADC_ClearFlag(ADCx_1, ADC_FLAG_EOC);
		Delay_ms(5000);
	}
}
