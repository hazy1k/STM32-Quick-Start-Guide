#ifndef __LED_H__
#define __LED_H__

#include "stm32f10x.h"

#define LED_GPIO_Port GPIOB
#define LED_GPIO_CLK RCC_APB2Periph_GPIOB
#define RED_LED_Pin GPIO_Pin_5
#define GREEN_LED_Pin GPIO_Pin_0
#define BLUE_LED_Pin GPIO_Pin_1

#define LED_ON(x)   GPIO_ResetBits(LED_GPIO_Port, (x))
#define LED_OFF(x)  GPIO_SetBits(LED_GPIO_Port, (x))

void BSP_LED_Init(void);

#endif /* __LED_H__ */
