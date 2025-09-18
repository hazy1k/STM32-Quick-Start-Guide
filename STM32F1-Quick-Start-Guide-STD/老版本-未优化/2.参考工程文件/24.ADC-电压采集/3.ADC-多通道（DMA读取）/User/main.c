#include "stm32f10x.h"
#include "usart.h"
#include "adc.h"
#include "SysTick.h"

extern __IO uint16_t ADC_ConvertedValue[CH_Num];	 
float ADC_Result[CH_Num];        

int main(void)
{		
	USART_Config();	
	SysTick_Init();
	ADCx_Init();
	while (1)
	{	
    	ADC_Result[0] = (float)ADC_ConvertedValue[0]/4096*3.3;
	 	ADC_Result[1] = (float)ADC_ConvertedValue[1]/4096*3.3;
		ADC_Result[2] = (float)ADC_ConvertedValue[2]/4096*3.3;
		ADC_Result[3] = (float)ADC_ConvertedValue[3]/4096*3.3;
		ADC_Result[4] = (float)ADC_ConvertedValue[4]/4096*3.3;
		ADC_Result[5] = (float)ADC_ConvertedValue[5]/4096*3.3;
		printf("\r\n CH0(PC0) value = %f V \r\n",ADC_Result[0]);
		printf("\r\n CH1(PC1) value = %f V \r\n",ADC_Result[1]);
		printf("\r\n CH2(PC2) value = %f V \r\n",ADC_Result[2]);
		printf("\r\n CH3(PC3) value = %f V \r\n",ADC_Result[3]);
		printf("\r\n CH4(PC4) value = %f V \r\n",ADC_Result[4]);
		printf("\r\n CH5(PC5) value = %f V \r\n",ADC_Result[5]);
		Delay_ms(5000);		 
	}
}
