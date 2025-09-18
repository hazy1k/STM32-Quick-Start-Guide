#ifndef __IWDG_H__
#define __IWDG_H__

#include "stm32f10x.h"

void IWDG_Init(uint8_t prv, uint16_t rld);
void IWDG_Feed(void);

#endif /* __IWDG_H__ */
