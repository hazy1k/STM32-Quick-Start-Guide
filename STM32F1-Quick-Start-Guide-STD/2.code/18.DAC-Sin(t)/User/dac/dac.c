#include "dac.h"

// 正弦波单个周期的点数
#define DAC_Point_Num 32

const uint16_t Single12Bit[DAC_Point_Num] = {
    2048,2460,2856,3218,3532,3786,3969,4072,
	4093,4031,3887,3668,3382,3042,2661,2255, 
	1841,1435,1054,714,428,209,65,3,
	24, 127,310,564,878,1240,1636,2048
};

uint32_t Double12Bit[DAC_Point_Num];

static void DAC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitStructure; 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_Cmd(DAC_Channel_2, ENABLE);
    DAC_DMACmd(DAC_Channel_2, ENABLE);
}

static void DAC_TIM_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = (20-1);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
    TIM_Cmd(TIM2, ENABLE);
}

static void DAC_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (DAC_BASE+0x20);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Double12Bit;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = DAC_Point_Num;
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

void DAC_Mode_Init(void)
{
    uint32_t Idx = 0;
    DAC_GPIO_Init();
    DAC_TIM_Init();
    for(Idx = 0; Idx < DAC_Point_Num; Idx++)
    {
        Double12Bit[Idx] = (Single12Bit[Idx] << 16) + (Single12Bit[Idx]);
    }
    DAC_DMA_Init();
}
