#include "stm32f10x.h"
#include "ADC.h"
#include "usart.h"
#include "SysTick.h"

extern __IO uint16_t ADC_ConvertedValue;
uint16_t ADC_Result;
uint16_t ADC_Result1;

int main(void)
{
	ADCx_Init();
	USART_Config();
	SysTick_Init();
	while(1)
	{
		Delay_ms(2000);
		ADC_Result1 = (V25-ADC_ConvertedValue)/ADC_K+25;
		ADC_Result = (1.43-ADC_ConvertedValue*3.3/4096)*1000/4.3+25;
		printf("ADC内部传感器温度: %3d °C\r\n", ADC_Result);
		printf("ADC Result: %d °C\r\n",ADC_Result1);
	}
}

