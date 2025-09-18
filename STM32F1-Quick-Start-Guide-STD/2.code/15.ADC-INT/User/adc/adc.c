#include "adc.h"

__IO uint16_t ADC_ConverValue; // ADC转换值

static void ADC_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void ADC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入
    GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
}

void ADC_Mode_Init(void)
{
    ADC_GPIO_Init();
    ADC_NVIC_Init();
    ADC_InitTypeDef ADC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    /* ADC Mode 配置 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // 独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; // 禁止扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // 连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 无外部触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 
    ADC_InitStructure.ADC_NbrOfChannel = 1; // 1个通道
    ADC_Init(ADCx, &ADC_InitStructure);
    /* 时钟配置 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8); // 72/8 = 9MHz
    ADC_RegularChannelConfig(ADCx, ADCx_Channel, 1, ADC_SampleTime_55Cycles5); // 配置ADC通道，采样时间为55.5个时钟周期
    /* 中断配置与初始化 */
    ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE); // 转换结果产生时中断
    ADC_Cmd(ADCx, ENABLE);
    ADC_ResetCalibration(ADCx); // 重置校准
    while (ADC_GetResetCalibrationStatus(ADCx)); // 等待重置完成
    ADC_StartCalibration(ADCx); // 开始校准
    while (ADC_GetCalibrationStatus(ADCx)); // 等待校准完成
    ADC_SoftwareStartConvCmd(ADCx, ENABLE); // 开始转换   
}

void ADC_IRQHandler(void)
{
    if(ADC_GetITStatus(ADCx, ADC_IT_EOC) != RESET) // 转换完成中断
    {
        ADC_ConverValue = ADC_GetConversionValue(ADCx); // 获取转换值
        LED_TOGGLE(BLUE_LED_Pin);
    }   
    ADC_ClearITPendingBit(ADCx, ADC_IT_EOC); // 清中断标志位
}
