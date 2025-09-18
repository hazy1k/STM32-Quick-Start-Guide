#include "ADC.h"

__IO uint16_t ADC_ConvertedValue; // 全局变量，存放ADC转换值

static void ADC_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_GPIO_APBxClock(ADC_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 单通道输入模式
	GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
}

static void ADC_Mode_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_APBxClock(ADC_CLK, ENABLE);
	/* ADC模式配置 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // 独立模式，只用到了一个ADC通道
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; // 不使用扫描模式，多通道才使用
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // 连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 禁止外部触发，使用软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1; // 单通道模式
	ADC_Init(ADCx, &ADC_InitStructure);
	/* ADC时钟配置 */
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); // 设置ADC时钟为PLCK2的8分频，即72/8=9MHz
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5); // 配置ADC通道，采样时间为55.5个时钟周期
	/* ADC中断配置 */
	ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE); // 转换结果产生时发生中断，在服务函数中计算转换值
	ADC_Cmd(ADCx, ENABLE); // 使能ADC
	ADC_ResetCalibration(ADCx); // 复位校准寄存器
	while(ADC_GetResetCalibrationStatus(ADCx)); // 等待校准完成
	ADC_StartCalibration(ADCx); // 开始校准
	while(ADC_GetCalibrationStatus(ADCx)); // 等待校准完成
	ADC_SoftwareStartConvCmd(ADCx, ENABLE); // 启动软件转换
}

void ADCx_Init(void)
{
	ADC_NVIC_Init();
	ADC_GPIO_Init();
	ADC_Mode_Init();
}
