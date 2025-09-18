#ifndef _LED_H
#define _LED_H

#include "sys.h"

#define LED0_GPIO GPIOF
#define LED0_PIN GPIO_PIN_9
#define LED1_GPIO GPIOF
#define LED1_PIN GPIO_PIN_10

// 定义LED状态
#define LED0_ON() HAL_GPIO_WritePin(LED0_GPIO, LED0_PIN, GPIO_PIN_RESET) 
#define LED0_OFF() HAL_GPIO_WritePin(LED0_GPIO, LED0_PIN, GPIO_PIN_SET) 
#define LED1_ON() HAL_GPIO_WritePin(LED1_GPIO, LED1_PIN, GPIO_PIN_RESET) 
#define LED1_OFF() HAL_GPIO_WritePin(LED1_GPIO, LED1_PIN, GPIO_PIN_SET)

// LED位操作函数
#define LED0 PFout(9)
#define LED1 PFout(10)

void LED_Init(void);

#endif
