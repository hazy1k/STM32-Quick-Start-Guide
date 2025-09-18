#ifndef __ADC3_H
#define __ADC3_H
#include "sys.h"

void Adc3_Init(void);  // ADC通道初始化
u16  Get_Adc3(u32 ch); // 获得某个通道值 

#endif 
