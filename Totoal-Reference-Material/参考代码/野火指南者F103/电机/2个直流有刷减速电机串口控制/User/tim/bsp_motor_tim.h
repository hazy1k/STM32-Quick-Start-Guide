#ifndef __BSP_MOTOR_TIM_H
#define	__BSP_MOTOR_TIM_H

#include "stm32f1xx.h"

/* 电机 1 相关定义 */
/*宏定义*/
#define PWM_TIM                        	TIM1
#define PWM_TIM_GPIO_AF_ENALBE()        __HAL_AFIO_REMAP_TIM1_ENABLE();
#define PWM_TIM_CLK_ENABLE()  					__HAL_RCC_TIM1_CLK_ENABLE()

#define PWM_CHANNEL_1                   TIM_CHANNEL_1
#define PWM_CHANNEL_2                   TIM_CHANNEL_2

/* 累计 TIM_Period个后产生一个更新或者中断*/		
/* 当定时器从0计数到PWM_PERIOD_COUNT，即为PWM_PERIOD_COUNT+1次，为一个定时周期 */
#define PWM_PERIOD_COUNT     (3600)

/* 通用控制定时器时钟源TIMxCLK = HCLK=72MHz */
/* 设定定时器频率为=TIMxCLK/(PWM_PRESCALER_COUNT+1) */
#define PWM_PRESCALER_COUNT     (2)

/* 最大比较值 */
#define PWM_MAX_PERIOD_COUNT              (PWM_PERIOD_COUNT - 100)

/*PWM引脚*/
#define PWM_TIM_CH1_GPIO_CLK()             __HAL_RCC_GPIOE_CLK_ENABLE();
#define PWM_TIM_CH1_GPIO_PORT              GPIOE
#define PWM_TIM_CH1_PIN                    GPIO_PIN_9

#define PWM_TIM_CH2_GPIO_CLK()             __HAL_RCC_GPIOE_CLK_ENABLE();
#define PWM_TIM_CH2_GPIO_PORT              GPIOE
#define PWM_TIM_CH2_PIN                    GPIO_PIN_11

/* 电机 2 相关定义 */
/*宏定义*/
#define PWM2_TIM                        TIM3
#define PWM2_TIM_GPIO_AF_ENALBE()        __HAL_AFIO_REMAP_TIM3_ENABLE();
#define PWM2_TIM_CLK_ENABLE()  					__HAL_RCC_TIM3_CLK_ENABLE()

#define PWM2_CHANNEL_1                   TIM_CHANNEL_1
#define PWM2_CHANNEL_2                   TIM_CHANNEL_2

/* 累计 TIM_Period个后产生一个更新或者中断*/		
/* 当定时器从0计数到PWM_PERIOD_COUNT，即为PWM_PERIOD_COUNT+1次，为一个定时周期 */
#define PWM2_PERIOD_COUNT     (3600)

/* 通用控制定时器时钟源TIMxCLK = HCLK=72MHz */
/* 设定定时器频率为=TIMxCLK/(PWM_PRESCALER_COUNT+1) */
#define PWM2_PRESCALER_COUNT     (2)

/* 最大比较值 */
#define PWM2_MAX_PERIOD_COUNT              (PWM2_PERIOD_COUNT - 100)

/*PWM引脚*/
#define PWM2_TIM_CH1_GPIO_CLK()          __HAL_RCC_GPIOC_CLK_ENABLE();
#define PWM2_TIM_CH1_GPIO_PORT           GPIOC
#define PWM2_TIM_CH1_PIN                 GPIO_PIN_6

#define PWM2_TIM_CH2_GPIO_CLK()          __HAL_RCC_GPIOC_CLK_ENABLE();
#define PWM2_TIM_CH2_GPIO_PORT           GPIOC
#define PWM2_TIM_CH2_PIN                 GPIO_PIN_7

extern TIM_HandleTypeDef  TIM_TimeBaseStructure;
extern TIM_HandleTypeDef  TIM_TimeBaseStructure2;

extern void TIMx_Configuration(void);
extern void TIM1_SetPWM_pulse(uint32_t channel,int compare);
extern void TIMx_Configuration2(void);
extern void TIM1_SetPWM2_pulse(uint32_t channel,int compare);

#endif   /* __BSP_MOTOR_TIM_H */
