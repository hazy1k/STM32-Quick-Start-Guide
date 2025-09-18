#ifndef __GENERALTIM_H
#define __GENERALTIM_H

#include "stm32f10x.h"

/* TIM基础配置 */
#define GTIMx TIM3
#define GTIM_APBxClock RCC_APB1PeriphClockCmd
#define GTIM_CLK RCC_APB1Periph_TIM3
#define GTIM_Period 9
#define GTIM_Prescaler 71

/* 四路输出通道配置 */
#define GTIM_CH1_GPIO_CLK RCC_APB2Periph_GPIOA
#define GTIM_CH1_GPIO_PORT GPIOA
#define GTIM_CH1_GPIO_PIN GPIO_Pin_6
#define GTIM_CH2_GPIO_CLK RCC_APB2Periph_GPIOA
#define GTIM_CH2_GPIO_PORT GPIOA
#define GTIM_CH2_GPIO_PIN GPIO_Pin_7
#define GTIM_CH3_GPIO_CLK RCC_APB2Periph_GPIOB
#define GTIM_CH3_GPIO_PORT GPIOB
#define GTIM_CH3_GPIO_PIN GPIO_Pin_0
#define GTIM_CH4_GPIO_CLK RCC_APB2Periph_GPIOB
#define GTIM_CH4_GPIO_PORT GPIOB
#define GTIM_CH4_GPIO_PIN GPIO_Pin_1

/* CCRx值 */
#define GTIM_CCR1 5
#define GTIM_CCR2 4
#define GTIM_CCR3 3
#define GTIM_CCR4 2

void GTIM_Init(void);

#endif /* __GENERALTIM_H */
