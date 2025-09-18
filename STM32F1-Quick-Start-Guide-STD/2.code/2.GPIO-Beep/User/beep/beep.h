#ifndef __BEEP_H__
#define __BEEP_H__ 

#include "stm32f10x.h"

#define Beep_GPIO_Port GPIOA
#define Beep_GPIO_CLK RCC_APB2Periph_GPIOA
#define Beep_GPIO_Pin GPIO_Pin_8

#define BEEP_ON() GPIO_SetBits(Beep_GPIO_Port, Beep_GPIO_Pin)
#define BEEP_OFF() GPIO_ResetBits(Beep_GPIO_Port, Beep_GPIO_Pin)

void BSP_Beep_Init(void);

#endif /* __BEEP_H__ */
