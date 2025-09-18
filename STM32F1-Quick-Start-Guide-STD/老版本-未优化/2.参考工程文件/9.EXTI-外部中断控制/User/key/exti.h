#ifndef __EXTI_H
#define __EXTI_H

#include "stm32f10x.h"

#define KEY1_GPIO_PORT GPIOA
#define KEY1_GPIO_PIN GPIO_Pin_0
#define KEY1_GPIO_CLK (RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO) // 使能GPIOA时钟和复用功能时钟
#define KEY1_EXTI_PORTSOURCE GPIO_PortSourceGPIOA // 外部中断端口
#define KEY1_EXTI_PINSOURCE GPIO_PinSource0 // 外部中断引脚
#define KEY1_EXTI_LINE EXTI_Line0 // 外部中断线
#define KEY1_EXTI_IRQ EXTI0_IRQn // 外部中断中断向量号
#define KEY1_IRQHandler EXTI0_IRQHandler // 外部中断服务函数

#define KEY2_GPIO_PORT GPIOC
#define KEY2_GPIO_PIN GPIO_Pin_13
#define KEY2_GPIO_CLK (RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO)
#define KEY2_EXTI_PORTSOURCE GPIO_PortSourceGPIOC 
#define KEY2_EXTI_PINSOURCE GPIO_PinSource13 
#define KEY2_EXTI_LINE EXTI_Line13 
#define KEY2_EXTI_IRQ EXTI15_10_IRQn 
#define KEY2_IRQHandler EXTI15_10_IRQHandler 

void NVIC_Configuration(void);
void EXTI_KeyInit(void);

#endif
