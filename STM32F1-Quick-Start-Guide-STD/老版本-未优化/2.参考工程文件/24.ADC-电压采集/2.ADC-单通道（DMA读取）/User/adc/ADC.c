#include "ADC.h"

__IO uint16_t ADC_ConvertedValue;

static void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_GPIO_APBxClock(ADC_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入模式
	GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
}

static void ADC_Mode_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	/* 时钟配置 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	ADC_APBxClock(ADC_CLK, ENABLE);
	/* DAM配置 */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx->DR)); // 外设地址为ADC数据寄存器地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue; // 内存地址为ADC_ConvertedValue变量地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // 数据源来自外设，也就是ADC
	DMA_InitStructure.DMA_BufferSize = 1; // 一次只传输一个数据
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; // 内存地址不自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
	/* ADC配置 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADCx, &ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); // 设置ADC时钟为SYSCLK/8 = 72/8 = 9MHz
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5); // 配置ADC通道，选择ADC_CHANNEL，转换时间为55个时钟周期
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
