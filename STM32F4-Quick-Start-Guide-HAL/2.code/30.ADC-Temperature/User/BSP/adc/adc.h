#ifndef __ADC_H__
#define __ADC_H__

#include <sys.h>

void adc_init(void);
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime);
uint32_t adc_get_result(uint32_t ch);
uint32_t adc_get_result_average(uint32_t ch, uint8_t times);
void adc_temp_init(void);
short adc_get_temp(void);

#endif /* __ADC_H__ */
