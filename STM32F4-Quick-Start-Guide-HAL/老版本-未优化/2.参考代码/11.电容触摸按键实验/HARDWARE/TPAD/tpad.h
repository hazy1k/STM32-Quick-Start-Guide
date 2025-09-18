#ifndef __TPAD_H
#define __TPAD_H

#include "sys.h"

void TIM2_CH1_Cap_Init(u32 arr, u16 psc);
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim);
void TPAD_Reset(void);
u16 TPAD_Get_Val(void);
u16 TPAD_Get_MaxVal(u8 n);
u8 TPAD_Scan(u8 mode);
uint8_t tpad_init(u16 psc);

#endif /* __TPAD_H */
