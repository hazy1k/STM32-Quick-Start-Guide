#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

/* ADC基础配置 */
#define ADC_APBxClock RCC_APB2PeriphClockCmd
#define ADC_CLK RCC_APB2Periph_ADC1
#define ADC_GPIO_APBxClock RCC_APB2PeriphClockCmd
#define ADC_GPIO_CLK RCC_APB2Periph_GPIOC
#define ADC_GPIO_PORT GPIOC
#define ADCx ADC1
#define ADC_DMA_CHANNEL DMA1_Channel1
#define ADC_DMA_CLK RCC_AHBPeriph_DMA1
/* 六路ADC通道配置 */
#define CH_Num 6
#define ADC_PIN1 GPIO_Pin_0
#define ADC_CHANNE1 ADC_Channel_10
#define ADC_PIN2 GPIO_Pin_1
#define ADC_CHANNE2 ADC_Channel_11
#define ADC_PIN3 GPIO_Pin_2
#define ADC_CHANNE3 ADC_Channel_12
#define ADC_PIN4 GPIO_Pin_3
#define ADC_CHANNE4 ADC_Channel_13
#define ADC_PIN5 GPIO_Pin_4
#define ADC_CHANNE5 ADC_Channel_14
#define ADC_PIN6 GPIO_Pin_5
#define ADC_CHANNE6 ADC_Channel_15

void ADCx_Init(void);

#endif /* __ADC_H */
