#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

void TIM14_PWM_Init(u16 arr, u16 psc);
void TIM_SetTIM14Compare1(u32 compare);

#endif /* __TIMER_H */
