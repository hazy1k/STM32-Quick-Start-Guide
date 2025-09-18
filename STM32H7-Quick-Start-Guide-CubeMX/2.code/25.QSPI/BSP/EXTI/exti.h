#ifndef __EXTI_H
#define __EXTI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"

#define EXTI_KEY1_Pin GPIO_PIN_1
#define EXTI_KEY1_GPIO_Port GPIOA
#define EXTI_KEY1_EXTI_IRQn EXTI1_IRQn

void MX_EXTI_GPIO_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __EXTI_H */
