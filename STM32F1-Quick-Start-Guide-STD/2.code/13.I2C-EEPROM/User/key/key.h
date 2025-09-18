#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"

#define KEY1_GPIO_CLK RCC_APB2Periph_GPIOA
#define KEY1_GPIO_Port GPIOA
#define KEY1_GPIO_Pin GPIO_Pin_0
#define KEY2_GPIO_CLK RCC_APB2Periph_GPIOC
#define KEY2_GPIO_Port GPIOC
#define KEY2_GPIO_Pin GPIO_Pin_13

#define KEY_OFF 0
#define KEY_ON 1

void BSP_KEY_Init(void);
uint8_t KEY_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif /* __KEY_H__ */
