#include "adc.h"

__IO uint16_t ADC_ConverValue[CH_Num];
const uint8_t adc_channels[CH_Num] = ADC_CHANNELS;

static void ADC_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
    GPIO_InitStruct.GPIO_Pin = ADC_PINS;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void ADC_Mode_Init(void) {
    DMA_InitTypeDef DMA_InitStruct = {0};
    ADC_InitTypeDef ADC_InitStruct = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    // DMA配置
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)ADC_ConverValue;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize = CH_Num;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStruct);
    DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);

    // ADC配置
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = ENABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = CH_Num;
    ADC_Init(ADCx, &ADC_InitStruct);

    // 配置每个通道
    for(uint8_t i=0; i<CH_Num; i++) {
        ADC_RegularChannelConfig(ADCx, adc_channels[i], i+1, ADC_SampleTime_55Cycles5);
    }

    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    ADC_DMACmd(ADCx, ENABLE);
    ADC_Cmd(ADCx, ENABLE);

    // 校准
    ADC_ResetCalibration(ADCx);
    while(ADC_GetResetCalibrationStatus(ADCx));
    ADC_StartCalibration(ADCx);
    while(ADC_GetCalibrationStatus(ADCx));
    
    ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}

void ADCx_DMA_Init(void) {
    ADC_GPIO_Init();
    ADC_Mode_Init();
}
