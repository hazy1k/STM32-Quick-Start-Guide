#ifndef __ADVANCETIME_H
#define __ADVANCETIME_H

#include "stm32f10x.h"

/* TIMx */
#define ATIMx TIM1
#define ATIM_APBxClock RCC_APB2PeriphClockCmd
#define ATIM_CLK RCC_APB2Periph_TIM1
/* PWN信号 */
// F = TIM_CLK/(ARR+1)*(PSC+1) = 72/8*9 = 1
#define ATIM_Period 7
#define ATIM_Prescaler 8
#define ATIM_Pulse 4
/* 输出比较通道 */
#define ATIM_CH1_GPIO_CLK RCC_APB2Periph_GPIOA // PA8
#define ATIM_CH1_GPIO_PORT GPIOA
#define ATIM_CH1_GPIO_PIN GPIO_Pin_8
/* 输出比通道的互补通道 */
#define ATIM_CH1N_GPIO_CLK RCC_APB2Periph_GPIOB // PB13
#define ATIM_CH1N_GPIO_PORT GPIOB
#define ATIM_CH1N_GPIO_PIN GPIO_Pin_13
/* 输出比较通道的刹车通道 */
#define ATIM_BKIN_GPIO_CLK RCC_APB2Periph_GPIOB // PB12
#define ATIM_BKIN_GPIO_PORT GPIOB
#define ATIM_BKIN_GPIO_PIN GPIO_Pin_12

void ATIMx_Config(void);

#endif	
