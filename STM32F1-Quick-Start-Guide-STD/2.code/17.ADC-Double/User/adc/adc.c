#include "ADC.h"

// 双ADC模式转换结果 (ADC2结果在高16位，ADC1结果在低16位)
__IO uint32_t ADC_ConvertedValue = 0;

static void ADC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // 使能GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    // 配置ADC1引脚 (PC1)
    GPIO_InitStruct.GPIO_Pin = ADC1_GPIO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入模式
    GPIO_Init(ADC1_GPIO_PORT, &GPIO_InitStruct);
    
    // 配置ADC2引脚 (PC4)
    GPIO_InitStruct.GPIO_Pin = ADC2_GPIO_PIN;
    GPIO_Init(ADC2_GPIO_PORT, &GPIO_InitStruct);
}

static void ADC_Mode_Init(void)
{
    DMA_InitTypeDef DMA_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;
    
    // 1. 时钟使能
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);      // DMA1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);   // ADC1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);   // ADC2时钟
    
    // 2. DMA配置
    DMA_DeInit(ADC_DMA_CHANNEL);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR); // 外设地址(ADC1数据寄存器)
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue; // 内存地址
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;         // 外设到内存
    DMA_InitStruct.DMA_BufferSize = 1;                      // 传输数据量(1个32位数据)
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址固定
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;   // 内存地址固定
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // 32位传输
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;   // 32位传输
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;            // 循环模式
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;        // 高优先级
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;               // 禁用内存到内存
    DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStruct);
    DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);                      // 使能DMA
    
    // 3. ADC公共配置
    ADC_InitStruct.ADC_ScanConvMode = ENABLE;               // 扫描模式
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;         // 连续转换
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;     // 数据右对齐
    ADC_InitStruct.ADC_NbrOfChannel = 1;                    // 1个转换通道
    
    // 4. ADC1配置 (主ADC)
    ADC_InitStruct.ADC_Mode = ADC_Mode_RegSimult;           // 同步规则模式
    ADC_Init(ADC1, &ADC_InitStruct);
    ADC_RegularChannelConfig(ADC1, ADC1_CHANNEL, 1, ADC_SampleTime_239Cycles5); // 通道11
    ADC_DMACmd(ADC1, ENABLE);                               // 使能DMA
    
    // 5. ADC2配置 (从ADC)
    ADC_InitStruct.ADC_Mode = ADC_Mode_RegSimult;           // 同步规则模式
    ADC_Init(ADC2, &ADC_InitStruct);
    ADC_RegularChannelConfig(ADC2, ADC2_CHANNEL, 1, ADC_SampleTime_239Cycles5); // 通道14
    ADC_ExternalTrigConvCmd(ADC2, ENABLE);                  // 使能外部触发
    
    // 6. 时钟配置 (8分频 = 72MHz/8 = 9MHz)
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    
    // 7. 校准ADC
    // ADC1校准
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    
    // ADC2校准
    ADC_Cmd(ADC2, ENABLE);
    ADC_ResetCalibration(ADC2);
    while(ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2));
    
    // 8. 启动转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 启动ADC1转换(同步触发ADC2)
}

void ADCx_Init(void)
{
    ADC_GPIO_Init();
    ADC_Mode_Init();
}
