#include "pvd.h"

void PVD_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* ÷–∂œ≈‰÷√ */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* EXTI≈‰÷√ */
  EXTI_ClearITPendingBit(EXTI_Line16);
  EXTI_InitStructure.EXTI_Line = EXTI_Line16;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  /* PVD≈‰÷√ */
  PWR_PVDLevelConfig(PWR_PVDLevel_2V6); // …Ë÷√µÁ—π„–÷µ
  PWR_PVDCmd(ENABLE);
}

