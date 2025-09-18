#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

void SysTick_Init(void);
void Delay_10us(__IO uint32_t nTime);;
void Delay_us(__IO uint32_t us);
void Delay_ms(__IO uint32_t ms);

#endif
