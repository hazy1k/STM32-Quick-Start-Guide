#ifndef __ADVANCE_TIM_H
#define	__ADVANCE_TIM_H

#include "stm32f1xx.h"

#define ADVANCE_TIM           		  TIM8
#define ADVANCE_TIM_CLK_ENABLE()  	__HAL_RCC_TIM8_CLK_ENABLE()

#define ADVANCE_TIM_IRQn		        TIM8_UP_IRQn
#define ADVANCE_TIM_IRQHandler      TIM8_UP_IRQHandler

extern TIM_HandleTypeDef TIM_TimeBaseStructure;

void TIMx_Configuration(void);

#endif /* __ADVANCE_TIM_H */

