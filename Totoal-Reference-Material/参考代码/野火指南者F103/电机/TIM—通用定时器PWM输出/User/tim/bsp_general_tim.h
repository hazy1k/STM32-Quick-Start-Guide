#ifndef __BSP_GENERAL_TIM_H
#define	__BSP_GENERAL_TIM_H

#include "stm32f1xx.h"


/*ºê¶¨Òå*/

#define GENERAL_TIM                        	TIM3
#define GENERAL_TIM_CLK_ENABLE()  					__HAL_RCC_TIM3_CLK_ENABLE()


/*PWMÒý½Å*/
#define GENERAL_TIM_CH1_GPIO_PORT           GPIOA
#define GENERAL_TIM_CH1_PIN                 GPIO_PIN_6

#define GENERAL_TIM_CH2_GPIO_PORT           GPIOA
#define GENERAL_TIM_CH2_PIN                 GPIO_PIN_7

extern void TIMx_Configuration(void);
extern void TIM2_SetPWM_pulse(int channel,int compare);

#endif
