#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "stm32f1xx.h"

#define BASIC_TIM           		  TIM6
#define BASIC_TIM_CLK_ENABLE()   	__HAL_RCC_TIM6_CLK_ENABLE()

#define BASIC_TIM_IRQn				    TIM6_DAC_IRQn
#define BASIC_TIM_IRQHandler    	TIM6_DAC_IRQHandler

/* 累计 TIM_Period个后产生一个更新或者中断*/		
//当定时器从0计数到BASIC_PERIOD_COUNT-1，即为BASIC_PERIOD_COUNT次，为一个定时周期

//PID计算周期:25ms计算一次--频率40Hz
#define BASIC_PERIOD_MS    (25)//25ms

#define BASIC_PERIOD_COUNT    (BASIC_PERIOD_MS*100)

//定时器时钟源TIMxCLK = 2 * APB1_CLK
//				如果APB1_CLK = 72MHz = TIMxCLK
//				否则TIMxCLK=APB1_CLK * 2
// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=1MHz
#define BASIC_PRESCALER_COUNT   (720)//100Khz

/* 获取定时器的周期，单位ms */
//#define __HAL_TIM_GET_PRESCALER(__HANDLE__)      ((__HANDLE__)->Instance->PSC)    // Get TIM Prescaler.
//#define GET_BASIC_TIM_PERIOD(__HANDLE__)    (1.0/(HAL_RCC_GetPCLK2Freq()/(__HAL_TIM_GET_PRESCALER(__HANDLE__)+1)/(__HAL_TIM_GET_AUTORELOAD(__HANDLE__)+1))*1000)

/* 以下两宏仅适用于定时器时钟源TIMxCLK=72MHz，预分频器为：7200-1 的情况 */
#define SET_BASIC_TIM_PERIOD(T)     __HAL_TIM_SET_AUTORELOAD(&TIM_TimeBaseStructure, (T)*100 - 1)    // 设置定时器的周期（1~1000ms）
#define GET_BASIC_TIM_PERIOD()      ((__HAL_TIM_GET_AUTORELOAD(&TIM_TimeBaseStructure)+1)/100.0f)     // 获取定时器的周期，单位ms

extern TIM_HandleTypeDef TIM_TimeBaseStructure;
void TIMx_Configuration(void);

#endif /* __BASIC_TIM_H */

