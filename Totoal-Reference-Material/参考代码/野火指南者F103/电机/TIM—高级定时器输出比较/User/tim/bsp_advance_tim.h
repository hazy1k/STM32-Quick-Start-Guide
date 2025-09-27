#ifndef __ADVANCE_TIM_H
#define	__ADVANCE_TIM_H

#include "stm32f1xx.h"

/* 高级定时器配置 */
/* 高级定时器配置 */
#define ADVANCE_TIM           		      TIM8
#define ADVANCE_TIM_CLK_ENABLE()  	    __HAL_RCC_TIM8_CLK_ENABLE()
#define ADVANCE_TIM_IRQn		            TIM8_CC_IRQn
#define ADVANCE_TIM_IRQHandler          TIM8_CC_IRQHandler

/* TIM8通道1输出引脚 */
#define CHANNEL1_OC_PIN           		  GPIO_PIN_6              
#define CHANNEL1_OC_GPIO_PORT     		  GPIOC                     
#define CHANNEL1_OC_GPIO_CLK_ENABLE() 	__HAL_RCC_GPIOC_CLK_ENABLE()
#define ADVANCE_TIM_CHANNEL_1           TIM_CHANNEL_1

/* TIM8通道2输出引脚 */
#define CHANNEL2_OC_PIN           		  GPIO_PIN_7              
#define CHANNEL2_OC_GPIO_PORT     		  GPIOC                     
#define CHANNEL2_OC_GPIO_CLK_ENABLE() 	__HAL_RCC_GPIOC_CLK_ENABLE()
#define ADVANCE_TIM_CHANNEL_2           TIM_CHANNEL_2

/* TIM8通道3输出引脚 */
#define CHANNEL3_OC_PIN           		  GPIO_PIN_8              
#define CHANNEL3_OC_GPIO_PORT     		  GPIOC                     
#define CHANNEL3_OC_GPIO_CLK_ENABLE() 	__HAL_RCC_GPIOC_CLK_ENABLE()
#define ADVANCE_TIM_CHANNEL_3           TIM_CHANNEL_3

/* TIM8通道4输出引脚 */
#define CHANNEL4_OC_PIN           		  GPIO_PIN_9              
#define CHANNEL4_OC_GPIO_PORT     		  GPIOC                     
#define CHANNEL4_OC_GPIO_CLK_ENABLE() 	__HAL_RCC_GPIOC_CLK_ENABLE()
#define ADVANCE_TIM_CHANNEL_4           TIM_CHANNEL_4

/*频率相关参数*/
//定时器实际时钟频率为：72MHz/(TIM_PRESCALER-1)
//168/(TIM_PRESCALER-1)=1Mhz
//具体需要的频率可以自己计算
#define TIM_PRESCALER                72
// 定义定时器周期，输出比较模式周期设置为0xFFFF
#define TIM_PERIOD                   0xFFFF

extern TIM_HandleTypeDef TIM_AdvanceHandle;

void TIMx_AdvanceConfig(void);

#endif /* __ADVANCE_TIM_H */


