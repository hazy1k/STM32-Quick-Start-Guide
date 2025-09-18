#ifndef __ADC_H__
#define __ADC_H__ 

#include "stm32f10x.h"

#define ADCx                   ADC1
#define ADC_GPIO_PORT          GPIOC
#define ADC_DMA_CHANNEL        DMA1_Channel1
#define CH_Num                 6

// 引脚到通道的映射
#define ADC_PINS (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5)
#define ADC_CHANNELS {ADC_Channel_10, ADC_Channel_11, ADC_Channel_12, \
                      ADC_Channel_13, ADC_Channel_14, ADC_Channel_15}

void ADCx_DMA_Init(void);
extern __IO uint16_t ADC_ConverValue[CH_Num];

#endif /* __ADC_H__ */
