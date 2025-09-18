#include "GeneralTim.h"

void TIMx_Init(void)
{
  /* NVIC中断优先级设置 */
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = TIM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* TIM MODE配置*/
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_TimeBaseStructure.TIM_Period = TIMx_Period; // 定时器周期999
  TIM_TimeBaseStructure.TIM_Prescaler = TIMx_Prescaler; // 定时器分频系数71
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频系数1
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIMx, TIM_FLAG_Update);
  TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE); // 使能计数器中断
  TIM_Cmd(TIMx, ENABLE);
}
