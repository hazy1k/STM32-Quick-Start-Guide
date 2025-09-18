#ifndef __BSP_TIMEBASE_H
#define __BSP_TIMEBASE_H

#include "stm32f10x.h"

/* 定时器基础配置 */
#define TIM_BASE TIM6
#define TIM_BASE_Period    999// ARR值
#define TIM_BASE_Prescaler 71 // 预分频系数
/* 中断相关 */
#define TIM_BASE_IRQ TIM6_IRQn
#define TIM_BASE_IRQHandler TIM6_IRQHandler

void TIM_BASE_Init(void);

#endif
