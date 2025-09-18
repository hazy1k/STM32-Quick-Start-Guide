#ifndef __TIM_H
#define __TIM_H

#include "stm32f10x.h"

void TIM_Mode_Init(void);
void TIM5_IRQHandler(void);
uint32_t Get_KeyPressDuration(void);
uint8_t Is_NewCaptureAvailable(void);

#endif /* __TIM_H */
