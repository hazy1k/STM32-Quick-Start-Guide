#ifndef __TIM_H
#define __TIM_H

#include "stm32f10x.h"

void TIM_Mode_Init(void);

extern volatile uint32_t time;

#endif /* __TIM_H */
