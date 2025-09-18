#include "AdvanceTim.h"

static void ATIM_NVIC_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = ATIM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

static void ATIM_Mode_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  TIM_TimeBaseStructure.TIM_Period = ATIM_Period; // 999
  TIM_TimeBaseStructure.TIM_Prescaler = ATIM_Prescaler; // 71
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(ATMx, &TIM_TimeBaseStructure);
  TIM_ClearFlag(ATMx, TIM_FLAG_Update); // 清除更新标志位
  TIM_ITConfig(ATMx, TIM_IT_Update, ENABLE); // 使能更新中断
  TIM_Cmd(ATMx, ENABLE); // 使能定时器
}

void ATIM_Config(void)
{
  ATIM_NVIC_Init();
  ATIM_Mode_Init();
}
