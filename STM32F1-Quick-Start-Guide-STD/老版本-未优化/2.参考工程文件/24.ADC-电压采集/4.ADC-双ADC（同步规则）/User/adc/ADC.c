#include "ADC.h"

__IO uint16_t ADC_ConvertedValue[1] = {0};

static void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* ADC1 GPIO Configuration */
    ADCx_1_GPIO_APBxClock(ADCx_1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADCx_1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(ADCx_1_GPIO_PORT, &GPIO_InitStructure);
	/* ADC2 GPIO Configuration */
	ADCx_2_GPIO_APBxClock(ADCx_2_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADCx_2_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(ADCx_2_GPIO_PORT, &GPIO_InitStructure);
}

static void ADC_Mode_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	/* ???? */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	ADCx_1_APBxClock(ADCx_1_CLK, ENABLE);
	ADCx_2_APBxClock(ADCx_2_CLK, ENABLE);
	/* DMA ?? */
	DMA_DeInit(ADC_DMA_CHANNEL);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx_1->DR));// ???ADC1?????
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue; // ????
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
	/* ADC1 ?? */
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult; // ?ADC????
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; // ????
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // ??????
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // ????????
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADCx_1, &ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	ADC_RegularChannelConfig(ADCx_1, ADCx_1_CHANNEL, 1, ADC_SampleTime_239Cycles5);
	ADC_DMACmd(ADCx_1, ENABLE);
	/* ADC2 ?? */
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADCx_2, &ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	ADC_RegularChannelConfig(ADCx_2, ADCx_2_CHANNEL, 1, ADC_SampleTime_239Cycles5);
	ADC_ExternalTrigConvCmd(ADCx_2, ENABLE); // ????????
	/* ADC ??*/
	ADC_Cmd(ADCx_1, ENABLE);
	ADC_ResetCalibration(ADCx_1);
	while(ADC_GetResetCalibrationStatus(ADCx_1));
	ADC_StartCalibration(ADCx_1);
	while(ADC_GetCalibrationStatus(ADCx_1));
	ADC_Cmd(ADCx_2, ENABLE);
	ADC_ResetCalibration(ADCx_2);
	while(ADC_GetResetCalibrationStatus(ADCx_2));
	ADC_StartCalibration(ADCx_2);
	while(ADC_GetCalibrationStatus(ADCx_2));
	ADC_SoftwareStartConvCmd(ADCx_1, ENABLE); // ??????
}

void ADCx_Init(void)
{
	ADC_GPIO_Init();
	ADC_Mode_Init();
}

