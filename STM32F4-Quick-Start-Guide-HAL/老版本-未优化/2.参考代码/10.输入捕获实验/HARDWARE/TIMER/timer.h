#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

extern TIM_HandleTypeDef TIM14_Handle; // 定时器14句柄
extern TIM_OC_InitTypeDef TIM14_CH1Handle; // 定时器14通道1句柄
extern TIM_HandleTypeDef TIM5_Handle; // 定时器5句柄

void TIM14_PWM_Init(uint16_t arr, uint16_t psc);
void TIM5_CH1_Cap_Init(uint32_t arr, uint16_t psc);
void TIM_SetTIM14Compare1(uint16_t compare);
uint32_t TIM_GetTIM14Capture1(void);

#endif /* __TIMER_H */
