#ifndef __ADVANCE_TIM_H
#define	__ADVANCE_TIM_H

#include "stm32f1xx.h"

/* 定时器 */
#define ADVANCE_TIM           				    TIM8
#define ADVANCE_TIM_CLK_ENABLE()  			  __HAL_RCC_TIM8_CLK_ENABLE()
 
/* TIM1通道1输出引脚 */
#define ADVANCE_OCPWM_PIN           		  GPIO_PIN_6             
#define ADVANCE_OCPWM_GPIO_PORT     		  GPIOC                    
#define ADVANCE_OCPWM_GPIO_CLK_ENABLE() 	__HAL_RCC_GPIOC_CLK_ENABLE()

/* TIM1通道1互补输出引脚 */
#define ADVANCE_OCNPWM_PIN            		GPIO_PIN_7              
#define ADVANCE_OCNPWM_GPIO_PORT      		GPIOA                     
#define ADVANCE_OCNPWM_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()

/* TIM1断路输入引脚 */
#define ADVANCE_BKIN_PIN              		GPIO_PIN_6              
#define ADVANCE_BKIN_GPIO_PORT        		GPIOA                      
#define ADVANCE_BKIN_GPIO_CLK_ENABLE()  	__HAL_RCC_GPIOA_CLK_ENABLE()


extern TIM_HandleTypeDef TIM_TimeBaseStructure;

void TIMx_Configuration(void);

#endif /* __ADVANCE_TIM_H */

