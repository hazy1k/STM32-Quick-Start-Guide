#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

#define ADC1_CHANNEL ADC_Channel_11
#define ADC2_CHANNEL ADC_Channel_14

#define ADC1_GPIO_PORT GPIOC
#define ADC1_GPIO_PIN  GPIO_Pin_1
#define ADC2_GPIO_PORT GPIOC
#define ADC2_GPIO_PIN  GPIO_Pin_4

#define ADC_DMA_CHANNEL DMA1_Channel1

void ADCx_Init(void);
extern __IO uint32_t ADC_ConvertedValue; // 双ADC模式结果存储

#endif /* __ADC_H */
