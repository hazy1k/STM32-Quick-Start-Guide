#ifndef __KEY_H__
#define __KEY_H__

#include "sys.h"
#include "delay.h"

/* 普通按键 */
#define KEY_GPIO_Port GPIOE
#define KEY_GPIO_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()
#define KEY0_GPIO_Pin GPIO_PIN_4 // KEY0 - PE4
#define KEY1_GPIO_Pin GPIO_PIN_3 // KEY1 - PE3
#define KEY2_GPIO_Pin GPIO_PIN_2 // KEY2 - PE2
/* UP按键 */
#define WKUP_GPIO_Port GPIOA
#define WKUP_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define WKUP_GPIO_Pin GPIO_PIN_0

/* 读取按键状态 */
#define KEY0_GET HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY0_GPIO_Pin)
#define KEY1_GET HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY1_GPIO_Pin)
#define KEY2_GET HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY2_GPIO_Pin)
#define WKUP_GET HAL_GPIO_ReadPin(WKUP_GPIO_Port, WKUP_GPIO_Pin)

#define KEY0_Press 1 // KEY0 按键按下
#define KEY1_Press 2 // KEY1 按键按下
#define KEY2_Press 3 // KEY2 按键按下
#define WKUP_Press 4 // WKUP 按键按下

void bsp_key_init(void);
uint8_t key_scan(uint8_t mode);

#endif /* __KEY_H__ */
