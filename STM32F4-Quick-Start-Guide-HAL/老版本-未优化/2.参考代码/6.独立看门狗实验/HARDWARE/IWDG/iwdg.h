#ifndef __IWDG_H
#define __IWDG_H
#include "sys.h"

void IWDG_Init(uint8_t prescaler, uint16_t reload);
void IWDG_Feed(void);

#endif
