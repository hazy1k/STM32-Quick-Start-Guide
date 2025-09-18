#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

// 12位的ADC，3.3V的ADC值为0xFFF, 温度25°时对应的电压值为1.43V即0x6EE
#define V25 0x6EE
// 斜率每摄氏度4.3mv，对应每摄氏度0x05
#define ADC_K 0x05
#define ADC1_DR_Address ((uint32_t)ADC1_BASE+0x4c)
void ADCx_Init(void);

#endif /* __ADC_H */
