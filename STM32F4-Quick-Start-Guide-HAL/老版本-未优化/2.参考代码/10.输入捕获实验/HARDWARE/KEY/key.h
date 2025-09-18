#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

// 各按键引脚宏定义
#define KEY0_GPIO GPIOE
#define KEY0_PIN GPIO_PIN_4
#define KEY1_GPIO GPIOE
#define KEY1_PIN GPIO_PIN_3
#define KEY2_GPIO GPIOE
#define KEY2_PIN GPIO_PIN_2
#define KEY_UP_GPIO GPIOA
#define KEY_UP_PIN GPIO_PIN_0

// 读取IO电平状态
#define KEY0_GET HAL_GPIO_ReadPin(KEY0_GPIO, KEY0_PIN)
#define KEY1_GET HAL_GPIO_ReadPin(KEY1_GPIO, KEY1_PIN)
#define KEY2_GET HAL_GPIO_ReadPin(KEY2_GPIO, KEY2_PIN)
#define KEY_UP_GET HAL_GPIO_ReadPin(KEY_UP_GPIO, KEY_UP_PIN)

// 宏定义按键值
#define KEY0_PRES 1
#define KEY1_PRES 2
#define KEY2_PRES 3
#define KEY_UP_PRES 4

void KEY_Init(void);
uint8_t KEY_Scan(uint8_t mode);

#endif
