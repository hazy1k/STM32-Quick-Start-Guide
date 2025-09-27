#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "stm32f1xx.h"

#define BASIC_TIM           		    TIM6
#define BASIC_TIM_CLK_ENABLE()     	__HAL_RCC_TIM6_CLK_ENABLE()

#define BASIC_TIM_IRQn				      TIM6_DAC_IRQn
#define BASIC_TIM_IRQHandler    	  TIM6_DAC_IRQHandler

extern TIM_HandleTypeDef TIM_TimeBaseStructure;
void TIMx_Configuration(void);

#endif /* __BASIC_TIM_H */

