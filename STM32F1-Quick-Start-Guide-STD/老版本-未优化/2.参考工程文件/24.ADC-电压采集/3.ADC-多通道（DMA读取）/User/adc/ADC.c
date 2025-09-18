#include "ADC.h"

__IO uint16_t ADC_ConvertedValue[CH_Num] = {0,0,0,0,0,0};

static void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_GPIO_APBxClock(ADC_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADC_PIN1|ADC_PIN2|ADC_PIN3|ADC_PIN4|ADC_PIN5|ADC_PIN6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
}

static void ADC_Mode_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	/*  ±÷”≈‰÷√ */
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
	ADC_APBxClock(ADC_CLK, ENABLE);
	DMA_DeInit(ADC_DMA_CHANNEL);
	/* DMA ≈‰÷√ */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx->DR));
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = CH_Num;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
	/* ADC ≈‰÷√ */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = CH_Num;
	ADC_Init(ADCx, &ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	ADC_RegularChannelConfig(ADCx, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_Channel_2, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_Channel_3, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_Channel_4, 4, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_Channel_5, 5, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_Channel_6, 6, ADC_SampleTime_55Cycles5);
	ADC_DMACmd(ADCx, ENABLE);
	ADC_Cmd(ADCx, ENABLE);
	ADC_ResetCalibration(ADCx);
	while(ADC_GetResetCalibrationStatus(ADCx));
	ADC_StartCalibration(ADCx);
	while(ADC_GetCalibrationStatus(ADCx));
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);
} 

void ADCx_Init(void)
{
	ADC_GPIO_Init();
	ADC_Mode_Init();
}
