#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

extern TIM_HandleTypeDef TIM3_Handler;      // ��ʱ��3��� 
extern TIM_HandleTypeDef TIM14_Handler;     // ��ʱ��14��� 
extern TIM_OC_InitTypeDef TIM14_CH1Handler; // ��ʱ��14ͨ��1���
extern TIM_HandleTypeDef TIM5_Handler;    	// ��ʱ��5��� 

void TIM3_Init(u16 arr,u16 psc);
void TIM14_PWM_Init(u16 arr,u16 psc);
void TIM_SetTIM14Compare1(u32 compare);
u32 TIM_GetTIM14Capture1(void);
void TIM5_CH1_Cap_Init(u32 arr,u16 psc);

#endif