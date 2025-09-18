#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

#define TIMx TIM3
#define TIM_IRQn TIM3_IRQn

void TIM_Config(uint16_t arr, uint16_t psc);

#endif /* __TIMER_H */
