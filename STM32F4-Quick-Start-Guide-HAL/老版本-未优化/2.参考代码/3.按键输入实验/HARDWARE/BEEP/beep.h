#ifndef __BEEP_H
#define __BEEP_H

#include "sys.h"

#define BEEP_GPIO GPIOF
#define BEEP_PIN GPIO_PIN_8
// 蜂鸣器高电平有效
#define BEEP_ON() HAL_GPIO_WritePin(BEEP_GPIO, BEEP_PIN, GPIO_PIN_SET)
#define BEEP_OFF() HAL_GPIO_WritePin(BEEP_GPIO, BEEP_PIN, GPIO_PIN_RESET)

void BEEP_Init(void);

#endif
