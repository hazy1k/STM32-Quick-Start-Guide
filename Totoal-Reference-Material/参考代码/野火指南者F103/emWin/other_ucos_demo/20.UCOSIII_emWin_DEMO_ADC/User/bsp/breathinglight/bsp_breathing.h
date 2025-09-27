#ifndef __PWM_BREATHING_H
#define	__PWM_BREATHING_H

#include "stm32f10x.h"

void TIM3_Breathing_Init(void);
void TIM3_Breathing_Close(void);
void SetRGBColor(uint32_t rgb);
void SetColorValue(uint8_t r,uint8_t g,uint8_t b);
#endif /* __PWM_BREATHING_H */

