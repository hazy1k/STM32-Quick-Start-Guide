#ifndef __GENERAL_TIM_H
#define __GENERAL_TIM_H

#include "stm32f10x.h"

/* TIM相关参数宏定义 */
#define TIMx TIM5
#define TIM_APBxClock RCC_APB1PeriphClockCmd
#define TIMx_CLK RCC_APB1Periph_TIM5
#define TIMx_Period 0xFFFF
#define TIMx_Prescaler 71
/* 输入捕获通道 */
#define TIM_CH1_GPIO_CLK RCC_APB2Periph_GPIOA
#define TIM_CH1_GPIO_PORT GPIOA
#define TIM_CH1_GPIO_PIN GPIO_Pin_0
#define TIM_CHx TIM_Channel_1
/* 中断配置 */
#define TIM_IT_CCx TIM_IT_CC1
#define TIM_IC_IRQ TIM5_IRQn
#define TIM_IC_IRQHandler TIM5_IRQHandler

void GTim_Init(void);

#endif /* __GENERAL_TIM_H */
