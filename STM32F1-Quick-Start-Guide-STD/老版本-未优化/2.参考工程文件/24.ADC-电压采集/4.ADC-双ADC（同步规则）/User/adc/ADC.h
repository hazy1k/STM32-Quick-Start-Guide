#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

/* ADC1通道 */
#define ADCx_1 ADC1
#define ADCx_1_APBxClock RCC_APB2PeriphClockCmd
#define ADCx_1_CLK RCC_APB2Periph_ADC1
#define ADCx_1_GPIO_APBxClock RCC_APB2PeriphClockCmd
#define ADCx_1_GPIO_CLK RCC_APB2Periph_GPIOC
#define ADCx_1_GPIO_PORT GPIOC
#define ADCx_1_GPIO_PIN GPIO_Pin_1
#define ADCx_1_CHANNEL ADC_Channel_11
/* ADC2通道 */
#define ADCx_2 ADC2
#define ADCx_2_APBxClock RCC_APB2PeriphClockCmd
#define ADCx_2_CLK RCC_APB2Periph_ADC2
#define ADCx_2_GPIO_APBxClock RCC_APB2PeriphClockCmd
#define ADCx_2_GPIO_CLK RCC_APB2Periph_GPIOC
#define ADCx_2_GPIO_PORT GPIOC
#define ADCx_2_GPIO_PIN GPIO_Pin_4
#define ADCx_2_CHANNEL ADC_Channel_14
/* DMA通道 */
#define ADC_DMA_CHANNEL DMA1_Channel1

void ADCx_Init(void);

#endif /* __ADC_H */
