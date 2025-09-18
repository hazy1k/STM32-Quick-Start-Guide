#ifndef __GENERAL_TIM_H
#define __GENERAL_TIM_H

#include "stm32f10x.h"

#define TIMx TIM2
#define TIMx_Period 999
#define TIMx_Prescaler 71
#define TIM_IRQ TIM2_IRQn
#define TIM_IRQHandler TIM2_IRQHandler
void TIMx_Init(void);

#endif /* __GENERAL_TIM_H */
