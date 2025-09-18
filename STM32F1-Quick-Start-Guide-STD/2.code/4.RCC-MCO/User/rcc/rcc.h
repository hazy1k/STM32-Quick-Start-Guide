#ifndef __RCC_H__
#define __RCC_H__

#include "stm32f10x.h"

void HSE_Clock_Init(uint32_t pllmul);
void HSI_Clock_Init(uint32_t pllmul);

#endif /* __RCC_H__ */
