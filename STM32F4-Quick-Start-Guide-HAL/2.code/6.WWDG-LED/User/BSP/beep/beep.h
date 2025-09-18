#ifndef __BEEP_H__
#define __BEEP_H__

#include "sys.h"

#define BEEP_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define BEEP_GPIO_Port GPIOF
#define BEEP_GPIO_Pin GPIO_PIN_8

#define BEEP_ON() HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_GPIO_Pin, GPIO_PIN_SET)
#define BEEP_OFF() HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_GPIO_Pin, GPIO_PIN_RESET)
#define BEEP_TOGGLE() HAL_GPIO_TogglePin(BEEP_GPIO_Port, BEEP_GPIO_Pin)

void bsp_beep_init(void);

#endif /* __BEEP_H__ */
