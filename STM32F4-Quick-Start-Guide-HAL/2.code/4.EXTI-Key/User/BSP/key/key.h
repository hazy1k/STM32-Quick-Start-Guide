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
#define WKUP_GPIO_Port GPIOA // WKUP - PA0
#define WKUP_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define WKUP_GPIO_Pin GPIO_PIN_0

/* 中断处理 */
#define KEY0_INT_IRQn EXTI4_IRQn
#define KEY0_INT_IRQHandler EXTI4_IRQHandler
#define KEY1_INT_IRQn EXTI3_IRQn
#define KEY1_INT_IRQHandler EXTI3_IRQHandler
#define KEY2_INT_IRQn EXTI2_IRQn
#define KEY2_INT_IRQHandler EXTI2_IRQHandler
#define WKUP_INT_IRQn EXTI0_IRQn
#define WKUP_INT_IRQHandler EXTI0_IRQHandler

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
void bsp_exti_init(void);

#endif /* __KEY_H__ */
