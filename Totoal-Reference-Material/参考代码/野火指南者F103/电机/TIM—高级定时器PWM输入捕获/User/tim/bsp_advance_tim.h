#ifndef __ADVANCE_TIM_H
#define	__ADVANCE_TIM_H

#include "stm32f1xx.h"


/* 通用定时器 */
#define GENERAL_TIM                       TIM3
#define GENERAL_TIM_CLK_ENABLE()          __HAL_RCC_TIM3_CLK_ENABLE()

/* 通用定时器PWM输出 */
/* PWM输出引脚 */
#define GENERAL_OCPWM_PIN                 GPIO_PIN_7              
#define GENERAL_OCPWM_GPIO_PORT           GPIOA                
#define GENERAL_OCPWM_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()

/* 高级控制定时器 */
#define ADVANCE_TIM                       TIM8
#define ADVANCE_TIM_CLK_ENABLE()          __HAL_RCC_TIM8_CLK_ENABLE()

/* 捕获/比较中断 */
#define ADVANCE_TIM_IRQn                  TIM8_CC_IRQn
#define ADVANCE_TIM_IRQHandler            TIM8_CC_IRQHandler
/* 高级控制定时器PWM输入捕获 */
/* PWM输入捕获引脚 */
#define ADVANCE_ICPWM_PIN                 GPIO_PIN_6              
#define ADVANCE_ICPWM_GPIO_PORT           GPIOC                    
#define ADVANCE_ICPWM_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADVANCE_IC1PWM_CHANNEL            TIM_CHANNEL_1
#define ADVANCE_IC2PWM_CHANNEL            TIM_CHANNEL_2

extern TIM_HandleTypeDef  TIM_PWMOUTPUT_Handle;
extern TIM_HandleTypeDef  TIM_PWMINPUT_Handle;

void TIMx_Configuration(void);
#endif /* __ADVANCE_TIM_H */


