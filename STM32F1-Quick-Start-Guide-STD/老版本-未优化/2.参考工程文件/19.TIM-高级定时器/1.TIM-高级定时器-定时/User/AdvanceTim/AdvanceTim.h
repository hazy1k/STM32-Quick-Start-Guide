#ifndef __ADVANCETIME_H
#define __ADVANCETIME_H

#include "stm32f10x.h"

#define ATMx TIM1
#define ATIM_Period 999
#define ATIM_Prescaler 71
#define ATIM_IRQ TIM1_UP_IRQn
#define ATIM_IRQHandler TIM1_UP_IRQHandler

void ATIM_Config(void);

#endif /* __ADVANCETIME_H */
