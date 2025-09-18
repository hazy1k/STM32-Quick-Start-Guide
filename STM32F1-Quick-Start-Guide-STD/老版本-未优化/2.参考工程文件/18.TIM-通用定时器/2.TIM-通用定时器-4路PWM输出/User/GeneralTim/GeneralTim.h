#ifndef __GENERAL_TIM_H
#define __GENERAL_TIM_H

#include "stm32f10x.h"
/* TIM相关参数宏定义 */
#define TIMx TIM3
#define TIM_APBxClock RCC_APB1PeriphClockCmd
#define TIMx_CLK RCC_APB1Periph_TIM3
#define TIMx_Period 9
#define TIMx_Prescaler 71

/* 四路输出通道宏定义 */
#define TIM_CH1_GPIO_CLK RCC_APB2Periph_GPIOA // TIM3_CH1 PA6
#define TIM_CH1_GPIO_PORT GPIOA
#define TIM_CH1_GPIO_PIN GPIO_Pin_6
#define TIM_CH2_GPIO_CLK RCC_APB2Periph_GPIOA // TIM3_CH2 PA7
#define TIM_CH2_GPIO_PORT GPIOA
#define TIM_CH2_GPIO_PIN GPIO_Pin_7
#define TIM_CH3_GPIO_CLK RCC_APB2Periph_GPIOB // TIM3_CH3 PB0
#define TIM_CH3_GPIO_PORT GPIOB
#define TIM_CH3_GPIO_PIN GPIO_Pin_0
#define TIM_CH4_GPIO_CLK RCC_APB2Periph_GPIOB // TIM3_CH4 PB1
#define TIM_CH4_GPIO_PORT GPIOB
#define TIM_CH4_GPIO_PIN GPIO_Pin_1

void GeneralTim_Init(void);

#endif /* __GENERAL_TIM_H */
