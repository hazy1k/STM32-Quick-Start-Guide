#include "dac.h"

#define PWM_Num 32 // 正弦波单个周期点数
// 波形数据
const uint16_t Sine12bit[PWM_Num] = {
	2048,2460,2856,3218,3532,3786,3969,4072,
	4093,4031,3887,3668,3382,3042,2661,2255, 
	1841,1435,1054,714,428,209,65,3,
	24,127,310,564,878,1240,1636,2048
};
// 双通道正弦波数据
uint32_t DualSine12bit[PWM_Num];

static void DAC_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef  DAC_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // 配置DAC 通道1
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;						// 使用TIM2作为触发源
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;	// 不使用波形发生器
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;	// 不使用DAC输出缓冲
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  // 配置DAC 通道2
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);
  DAC_Cmd(DAC_Channel_1, ENABLE);
  DAC_Cmd(DAC_Channel_2, ENABLE);
  DAC_DMACmd(DAC_Channel_2, ENABLE);
}

static void DAC_TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
  TIM_TimeBaseStructure.TIM_Period = (20-1);       			
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;       						
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    					
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
  TIM_Cmd(TIM2, ENABLE);
}

static void DAC_DMA_Config(void)
{	
	DMA_InitTypeDef  DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_ADDRESS;	 // 外设数据地址 寄存器 DHR12RD 的地址12位、右对齐、双通道
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DualSine12bit; // 内存数据地址 DualSine12bit
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;							 // 数据传输方向内存至外设
  DMA_InitStructure.DMA_BufferSize = PWM_Num;													
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;				
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;													
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;											
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;														
  DMA_Init(DMA2_Channel4, &DMA_InitStructure);
  DMA_Cmd(DMA2_Channel4, ENABLE);
}

// 初始化DAC模式
void DAC_Mode_Init(void)
{
	uint32_t Idx = 0;  
	DAC_Config();
	DAC_TIM_Config();	
	// 填充正弦波形数据，双通道右对齐
  for (Idx = 0; Idx < PWM_Num; Idx++)
  {
    DualSine12bit[Idx] = (Sine12bit[Idx] << 16) + (Sine12bit[Idx]);
  }
	DAC_DMA_Config();
}
