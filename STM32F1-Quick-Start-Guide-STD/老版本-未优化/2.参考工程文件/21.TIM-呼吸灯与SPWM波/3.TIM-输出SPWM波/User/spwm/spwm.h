#ifndef __SPWM_H
#define	__SPWM_H

#include "stm32f10x.h"

extern __IO uint16_t period_class ;
extern uint16_t PWM_Num;

#define AMPLITUDE_CLASS 256 // 电压幅值等级数

/* TIM相关参数宏定义 */
#define TIMx TIM3
#define TIM_APBxClock_FUN RCC_APB1PeriphClockCmd
#define TIM_CLK           RCC_APB1Periph_TIM3
#define TIM_GPIO_CLK      (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
#define GPIO_REMAP_FUN()  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 				

/* 三色灯通道定义 */
#define RED_TIM_LED_PORT GPIOB
#define RED_TIM_LED_PIN  GPIO_Pin_5
#define RED_TIM_OCxInit          TIM_OC2Init            
#define RED_TIM_OCxPreloadConfig TIM_OC2PreloadConfig
#define RED_CCRx CCR2		
#define GREEN_TIM_LED_PORT GPIOB
#define GREEN_TIM_LED_PIN  GPIO_Pin_0
#define GREEN_TIM_OCxInit          TIM_OC3Init            
#define GREEN_TIM_OCxPreloadConfig TIM_OC3PreloadConfig 
#define GREEN_CCRx CCR3
#define BLUE_TIM_LED_PORT GPIOB
#define BLUE_TIM_LED_PIN  GPIO_Pin_1
#define BLUE_TIM_OCxInit          TIM_OC4Init            
#define BLUE_TIM_OCxPreloadConfig TIM_OC4PreloadConfig  
#define BLUE_CCRx CCR4
/* 中断相关配置 */
#define TIMx_IRQ        TIM3_IRQn              
#define TIMx_IRQHandler TIM3_IRQHandler

void SPWM_Init(void);

#endif /* __SPWM_H */
