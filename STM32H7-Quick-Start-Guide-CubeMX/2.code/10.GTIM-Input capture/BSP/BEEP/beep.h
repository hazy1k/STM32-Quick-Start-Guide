#ifndef __BEEP_H
#define __BEEP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"

#define BEEP_Pin GPIO_PIN_4
#define BEEP_GPIO_Port GPIOE

void MX_BEEP_GPIO_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BEEP_H */
