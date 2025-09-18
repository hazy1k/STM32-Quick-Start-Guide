#ifndef __BREATHING_H
#define __BREATHING_H

#include "stm32f10x.h"

extern uint16_t PWM_Num;
extern __IO uint16_t period_class;

/* TIM≈‰÷√ */
#define RED_TIMx TIM3
#define RED_APBxClock RCC_APB1PeriphClockCmd
#define RED_TIM_CLK RCC_APB1Periph_TIM3
#define RED_TIM_GPIO_APBxClock RCC_APB2PeriphClockCmd
#define RED_TIM_GPIO_CLK (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
/* ∫Ïµ∆“˝Ω≈÷ÿ”≥…‰ */
#define RED_GPIO_REMAP_FUN() GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
#define RED_GPIO_PORT GPIOB
#define RED_GPIO_PIN GPIO_Pin_5
#define RED_TIM_OCxInit TIM_OC2Init
#define RED_TIM_OCxPreloadConfig TIM_OC2PreloadConfig
#define RED_CCRx CCR2
/* ÷–∂œ≈‰÷√ */
#define RED_TIM_IRQ TIM3_IRQn
#define RED_TIM_IRQHandler TIM3_IRQHandler

void breathing_init(void);

#endif /* __BREATHING_H */
