#ifndef __ADC_H
#define __ADC_H

#include "sys.h"

// 定义ADC相关的GPIO和ADC外设
#define SINGLE_ADC_GPIO_PORT              GPIOA
#define SINGLE_ADC_GPIO_PIN               GPIO_PIN_5
#define SINGLE_ADC_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define ADC_INSTANCE                      ADC1
#define ADC_CLK_ENABLE()                  __HAL_RCC_ADC1_CLK_ENABLE()

// 定义ADC DMA相关的宏
#define ADC_DMA_STREAM                    DMA2_Stream4
#define ADC_DMA_CHANNEL                   DMA_CHANNEL_0
#define ADC_DMA_IRQn                      DMA2_Stream4_IRQn

// 多通道ADC DMA采集通道数
#define MULTI_ADC_CHANNEL_NUM             6

// 外部声明
extern uint8_t g_adc_dma_transfer_complete_flag;
extern uint16_t g_adc_dma_buffer[]; // 声明外部缓冲区

void adc_multi_channel_dma_init(void);
void adc_multi_channel_dma_enable(uint16_t num_transfers);
void DMA2_Stream4_IRQHandler(void);

#endif /* __ADC_H */
