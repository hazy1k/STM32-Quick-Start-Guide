#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"

#define LED1_Pin GPIO_PIN_5  // 绿
#define LED1_GPIO_Port GPIOE
#define LED2_Pin GPIO_PIN_6  // 红
#define LED2_GPIO_Port GPIOE
#define LED0_Pin GPIO_PIN_4  // 蓝
#define LED0_GPIO_Port GPIOB

#define LED_RED_Pin    LED2_Pin
#define LED_RED_Port   LED2_GPIO_Port
#define LED_BLUE_Pin   LED0_Pin
#define LED_BLUE_Port  LED0_GPIO_Port
#define LED_GREEN_Pin  LED1_Pin
#define LED_GREEN_Port LED1_GPIO_Port

void MX_LED_GPIO_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __LED_H */
