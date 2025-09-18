#include "atim.h"

uint16_t ATIM_Buffer[ATIM_DMA_BufferSize] = {700, 700, 100};

static void ATIM_GPIO_Init(void)
{
  // CH1 GPIO Configuration
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(ATIM_CH1_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = ATIM_CH1_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(ATIM_CH1_GPIO_PORT, &GPIO_InitStructure);
  // DMA Configuration
  DMA_InitTypeDef DMA_InitStructure;
  RCC_AHBPeriphClockCmd(ATIM_DMA_CLK, ENABLE);
  DMA_DeInit(ATIM_DMA_Channel);
  // DMA配置
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(ATIM_DMA_ADDRESS); // 设置DMA源地址：TIM的CCR寄存器
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ATIM_Buffer; // 设置DMA目的地址：ATIM_Buffer
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; // 设置DMA传输方向：从内存到外设
  DMA_InitStructure.DMA_BufferSize = ATIM_DMA_BufferSize; // 设置DMA传输数据长度
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(ATIM_DMA_Channel, &DMA_InitStructure);
}

static void ATIM_Mode_Init(void)
{
  // 时基结构初始化
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB2PeriphClockCmd(ATIM_CLK, ENABLE);
  TIM_TimeBaseStructure.TIM_Period = 1000-1;
  TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(ATIMx, &TIM_TimeBaseStructure);
  // 输出PWM模式配置
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(ATIMx, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(ATIMx, TIM_OCPreload_Enable);
  TIM_Cmd(ATIMx, ENABLE);
  // DMA使能
  DMA_Cmd(ATIM_DMA_Channel, ENABLE);
  TIM_DMACmd(ATIMx, TIM_DMA_CC1, ENABLE);
  TIM_CtrlPWMOutputs(ATIMx, ENABLE); // 使能PWM输出
}

void ATIMx_Init(void)
{
  ATIM_GPIO_Init();
  ATIM_Mode_Init();
}
