#ifndef __KEY_H
#define __KEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"

#define WK_UP_Pin GPIO_PIN_0
#define WK_UP_GPIO_Port GPIOA
#define KEY0_Pin GPIO_PIN_1
#define KEY0_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_15
#define KEY1_GPIO_Port GPIOA

#define KEY0_PRES 1
#define KEY1_PRES 2
#define WKUP_PRES 3

void MX_KEY_GPIO_Init(void);
uint8_t key_scan(uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif /* __KEY_H */
