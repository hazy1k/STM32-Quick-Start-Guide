#ifndef __ADC_H__
#define __ADC_H__ 

#include "stm32f10x.h"
#include "led.h"

#define ADCx ADC2
#define ADCx_Channel ADC_Channel_11
#define ADC_GPIO_PORT GPIOC
#define ADC_GPIO_PIN GPIO_Pin_1
#define ADC_IRQn ADC1_2_IRQn
#define ADC_IRQHandler ADC1_2_IRQHandler

void ADC_Mode_Init(void);

#endif /* __ADC_H__ */
