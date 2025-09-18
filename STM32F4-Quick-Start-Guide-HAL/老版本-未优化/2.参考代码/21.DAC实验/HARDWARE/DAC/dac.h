#ifndef __DAC_H
#define __DAC_H
#include "sys.h"

extern DAC_HandleTypeDef DAC1_Handler;//DAC¾ä±ú

void DAC1_Init(void);
void DAC1_Set_Vol(u16 vol);

#endif
