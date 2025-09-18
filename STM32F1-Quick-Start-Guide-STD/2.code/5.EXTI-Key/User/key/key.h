#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"

/* KEY GPIO */
#define KEY1_GPIO_CLK RCC_APB2Periph_GPIOA // KEY1 PA0
#define KEY1_GPIO_Port GPIOA
#define KEY1_GPIO_Pin GPIO_Pin_0
#define KEY2_GPIO_CLK RCC_APB2Periph_GPIOC // KEY2 PC13
#define KEY2_GPIO_Port GPIOC
#define KEY2_GPIO_Pin GPIO_Pin_13

/* KEY EXTI */
#define KEY1_EXTI_CLK (RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO)
#define KEY1_EXTI_Port GPIO_PortSourceGPIOA // 外部中断端口
#define KEY1_EXTI_Pin GPIO_PinSource0 // 外部中断引脚
#define KEY1_EXTI_Line EXTI_Line0 // 外部中断线
#define KEY1_EXTI_IRQ EXTI0_IRQn // 外部中断中断号
#define KEY1_IRQHandler EXTI0_IRQHandler // 外部中断服务函数

#define KEY2_EXTI_CLK (RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO)
#define KEY2_EXTI_Port GPIO_PortSourceGPIOC
#define KEY2_EXTI_Pin GPIO_PinSource13
#define KEY2_EXTI_Line EXTI_Line13
#define KEY2_EXTI_IRQ EXTI15_10_IRQn
#define KEY2_IRQHandler EXTI15_10_IRQHandler

#define KEY_OFF 0
#define KEY_ON 1

void NVIC_Configuration(void);
void KEY_EXTI_Init(void);

#endif /* __KEY_H__ */
