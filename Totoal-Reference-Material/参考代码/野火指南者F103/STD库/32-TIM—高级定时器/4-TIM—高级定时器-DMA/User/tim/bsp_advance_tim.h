#ifndef __ADVANCE_TIM_H
#define	__ADVANCE_TIM_H

#include "stm32f10x.h"

/* 定时器 */
#define ADVANCE_TIM           		    TIM1
#define ADVANCE_TIM_CLK       		    RCC_APB2Periph_TIM1
                           

// TIM1 输出比较通道
#define ADVANCE_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define ADVANCE_TIM_CH1_PORT          GPIOA
#define ADVANCE_TIM_CH1_PIN           GPIO_Pin_8

/* DMA  */
#define TIM_DMA_ADDRESS               TIM1_BASE+0x34
#define TIM_DMA_BUFSIZE               3
#define TIM_DMA_CLK                   RCC_AHBPeriph_DMA1
#define TIM_DMA_STREAM                DMA1_Channel2

void TIMx_Configuration(void);

#endif /* __ADVANCE_TIM_H */

