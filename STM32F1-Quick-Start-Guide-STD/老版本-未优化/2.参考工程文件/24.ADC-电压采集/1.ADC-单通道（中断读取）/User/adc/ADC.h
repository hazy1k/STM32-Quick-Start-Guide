#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

/* ADC通道相关参数定义 */
#define ADC_GPIO_APBxClock RCC_APB2PeriphClockCmd
#define ADC_GPIO_CLK RCC_APB2Periph_GPIOC
#define ADC_GPIO_PORT GPIOC
#define ADC_GPIO_PIN GPIO_Pin_1
#define ADC_APBxClock RCC_APB2PeriphClockCmd
#define ADCx ADC2
#define ADC_CLK RCC_APB2Periph_ADC2
#define ADC_CHANNEL ADC_Channel_11
/* 中断相关 */
#define ADC_IRQ ADC1_2_IRQn
#define ADC_IRQHandler ADC1_2_IRQHandler

void ADCx_Init(void);

#endif /* __ADC_H */
