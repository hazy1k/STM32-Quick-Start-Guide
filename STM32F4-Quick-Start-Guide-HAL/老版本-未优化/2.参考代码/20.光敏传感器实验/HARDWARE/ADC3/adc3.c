#include "adc3.h"
#include "delay.h"

ADC_HandleTypeDef ADC3_Handler;//ADC句柄
// 初始化ADC
// ch: ADC_channels 
// 通道值 0~16取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
void Adc3_Init(void)
{   
    ADC3_Handler.Instance=ADC3;
    ADC3_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; // 4分频，ADCCLK=PCLK2/4=84/4=21MHZ
    ADC3_Handler.Init.Resolution=ADC_RESOLUTION_12B;           // 12位模式
    ADC3_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;           // 右对齐
    ADC3_Handler.Init.ScanConvMode=DISABLE;                    // 非扫描模式
    ADC3_Handler.Init.EOCSelection=DISABLE;                    // 关闭EOC中断
    ADC3_Handler.Init.ContinuousConvMode=DISABLE;              // 关闭连续转换
    ADC3_Handler.Init.NbrOfConversion=1;                       // 1个转换在规则序列中 也就是只转换规则序列1 
    ADC3_Handler.Init.DiscontinuousConvMode=DISABLE;           // 禁止不连续采样模式
    ADC3_Handler.Init.NbrOfDiscConversion=0;                   // 不连续采样通道数为0
    ADC3_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;     // 软件触发
    ADC3_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;// 使用软件触发
    ADC3_Handler.Init.DMAContinuousRequests=DISABLE;           // 关闭DMA请求
    HAL_ADC_Init(&ADC3_Handler);                               // 初始化     
}

// ADC底层驱动，引脚配置，时钟使能
// 此函数会被HAL_ADC_Init()调用
// hadc:ADC句柄
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    __HAL_RCC_ADC3_CLK_ENABLE();   // 使能ADC3时钟
}
// 获得ADC值
// ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
// 返回值:转换结果
u16 Get_Adc3(u32 ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf; 
    ADC1_ChanConf.Channel=ch;                            // 通道
    ADC1_ChanConf.Rank=1;                                // 1个序列
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES; // 采样时间
    ADC1_ChanConf.Offset=0;                              // 偏移量为0
    HAL_ADC_ConfigChannel(&ADC3_Handler,&ADC1_ChanConf); // 通道配置
    HAL_ADC_Start(&ADC3_Handler);                        // 开启ADC
    HAL_ADC_PollForConversion(&ADC3_Handler,10);         // 轮询转换
	return (u16)HAL_ADC_GetValue(&ADC3_Handler);	     // 返回最近一次ADC1规则组的转换结果
}
