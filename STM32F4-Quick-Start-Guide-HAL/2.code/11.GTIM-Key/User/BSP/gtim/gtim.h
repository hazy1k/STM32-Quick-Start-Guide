#ifndef __GTIME_H__
#define __GTIME_H__ 

#include "sys.h"

void TIM2_Mode_Init(uint16_t psc);
uint32_t TIM2_CH1_GetCount(void);
void TIM2_CH1_Restart(void);
void TIM2_IRQHandler(void);

#endif /* __GTIME_H__ */