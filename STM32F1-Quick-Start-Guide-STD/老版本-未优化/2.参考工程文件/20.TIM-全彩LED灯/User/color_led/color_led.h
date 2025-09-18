#ifndef __COLOR_LED_H
#define __COLOR_LED_H

#include "stm32f10x.h"

/* TIMx */
#define TIMx TIM3
#define TIM_APBxClock RCC_APB1PeriphClockCmd
#define TIM_CLK RCC_APB1Periph_TIM3
#define TIM_GPIO_CLK (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)

// RED
#define GPIO_REMAP_FUN() GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); // TIM3重映射
#define RED_GPIO_PORT GPIOB
#define RED_GPIO_PIN GPIO_Pin_5
#define RED_OCxInit TIM_OC2Init  // 通道初始化函数
#define RED_OCxPreloadConfig TIM_OC2PreloadConfig  // 预装载配置函数
#define RED_CCRx CCR2
// GREEN
#define GREEN_GPIO_PORT GPIOB
#define GREEN_GPIO_PIN GPIO_Pin_0
#define GREEN_OCxInit TIM_OC3Init  
#define GREEN_OCxPreloadConfig TIM_OC3PreloadConfig 
#define GREEN_CCRx CCR3
// BLUE
#define BLUE_GPIO_PORT GPIOB
#define BLUE_GPIO_PIN GPIO_Pin_1
#define BLUE_OCxInit TIM_OC4Init  
#define BLUE_OCxPreloadConfig TIM_OC4PreloadConfig 
#define BLUE_CCRx CCR4

void TIMx_Init(void);
void SetRGBColor(uint32_t rgb);
void SetColorValue(uint8_t r, uint8_t g, uint8_t b);
void LED_Close(void);

#endif /* __COLOR_LED_H */
