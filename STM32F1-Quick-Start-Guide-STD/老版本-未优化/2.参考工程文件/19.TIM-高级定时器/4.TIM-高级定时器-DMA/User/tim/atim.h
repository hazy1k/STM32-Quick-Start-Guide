#ifndef __ATIM_H
#define __ATIM_H

#include "stm32f10x.h"

/* TIM1输出比较通道1 */
#define ATIMx TIM1
#define ATIM_CLK RCC_APB2Periph_TIM1
#define ATIM_CH1_GPIO_CLK RCC_APB2Periph_GPIOA
#define ATIM_CH1_GPIO_PORT GPIOA
#define ATIM_CH1_GPIO_PIN GPIO_Pin_8

/* DMA配置*/
#define ATIM_DMA_ADDRESS TIM1_BASE+0x34
#define ATIM_DMA_BufferSize 3
#define ATIM_DMA_CLK RCC_AHBPeriph_DMA1
#define ATIM_DMA_Channel DMA1_Channel2

void ATIMx_Init(void);

#endif /* __ATIM_H */
