#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "stm32f10x.h"

void SysTick_Init(void);
void Delay_10us(__IO uint32_t nTime);
void Delay_us(__IO uint32_t us);
void Delay_ms(__IO uint32_t ms);
void SysTick_Handler(void);

#endif /* __SYSTICK_H__ */
