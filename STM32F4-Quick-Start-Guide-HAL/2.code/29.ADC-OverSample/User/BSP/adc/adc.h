#ifndef __ADC_H__
#define __ADC_H__

#include "sys.h"

#define ADC_DMA_BUFFSIZE 2560 // 定义DMA缓冲区大小

void adc_init(void);
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime);
uint32_t adc_get_result(uint32_t ch);
uint32_t adc_get_result_average(uint32_t ch, uint8_t times);
void adc_dma_init(uint32_t address);
void adc_dma_enable(uint16_t times); // 这个函数在DMA_CIRCULAR模式下可能作用不大，但保留以兼容main函数

extern uint8_t adc_dma_sta;

#endif /* __ADC_H__ */
