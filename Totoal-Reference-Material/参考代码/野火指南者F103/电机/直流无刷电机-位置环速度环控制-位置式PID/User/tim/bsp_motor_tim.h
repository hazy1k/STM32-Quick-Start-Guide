#ifndef __BSP_MOTOR_TIM_H
#define	__BSP_MOTOR_TIM_H

#include "stm32f1xx.h"
#include "./led/bsp_led.h"   
#include ".\bldcm_control\bsp_bldcm_control.h"
#include <stdio.h>

/* 电机控旋转实现结构体 */

#define SPEED_FILTER_NUM      25    // 速度滤波次数

typedef struct
{
  int32_t timeout;            // 定时器更新计数
  float speed;                // 电机速度 rps（转/分钟）
  int32_t enable_flag;        // 电机使能标志
  int32_t speed_group[SPEED_FILTER_NUM];
  int32_t location;
}motor_rotate_t;

/* 电机控制定时器 */
#define MOTOR_TIM           				      TIM1
#define MOTOR_TIM_CLK_ENABLE()  			    __HAL_RCC_TIM1_CLK_ENABLE()
extern TIM_HandleTypeDef  htimx_bldcm;

/* 累计 TIM_Period个后产生一个更新或者中断		
	当定时器从0计数到2399，即为2400次，为一个定时周期 */
#define PWM_PERIOD_COUNT     (2400)

#define PWM_MAX_PERIOD_COUNT    (PWM_PERIOD_COUNT - 100)

/* 高级控制定时器时钟源TIMxCLK = HCLK = 72MHz 
	 设定定时器频率为=TIMxCLK/(PWM_PRESCALER_COUNT)/PWM_PERIOD_COUNT = 15KHz*/
#define PWM_PRESCALER_COUNT     (2)

#define MOTOR_OCPWM1_AF_ENABLE()          __HAL_AFIO_REMAP_TIM1_ENABLE();

/* TIM1通道1输出引脚 */
#define MOTOR_OCPWM1_PIN           		    GPIO_PIN_9
#define MOTOR_OCPWM1_GPIO_PORT     		    GPIOE
#define MOTOR_OCPWM1_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOE_CLK_ENABLE()

/* TIM1通道2输出引脚 */
#define MOTOR_OCPWM2_PIN           		    GPIO_PIN_11
#define MOTOR_OCPWM2_GPIO_PORT     		    GPIOE
#define MOTOR_OCPWM2_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOE_CLK_ENABLE()

/* TIM1通道3输出引脚 */
#define MOTOR_OCPWM3_PIN           		    GPIO_PIN_13
#define MOTOR_OCPWM3_GPIO_PORT     		    GPIOE
#define MOTOR_OCPWM3_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOE_CLK_ENABLE()

/* TIM1通道1互补输出引脚 */
#define MOTOR_OCNPWM1_PIN            		  GPIO_PIN_8
#define MOTOR_OCNPWM1_GPIO_PORT      		  GPIOE
#define MOTOR_OCNPWM1_GPIO_CLK_ENABLE()	  __HAL_RCC_GPIOE_CLK_ENABLE()

/* TIM1通道2互补输出引脚 */
#define MOTOR_OCNPWM2_PIN            		  GPIO_PIN_10
#define MOTOR_OCNPWM2_GPIO_PORT      		  GPIOE
#define MOTOR_OCNPWM2_GPIO_CLK_ENABLE()	  __HAL_RCC_GPIOE_CLK_ENABLE()

/* TIM1通道3互补输出引脚 */
#define MOTOR_OCNPWM3_PIN            		  GPIO_PIN_12
#define MOTOR_OCNPWM3_GPIO_PORT      		  GPIOE
#define MOTOR_OCNPWM3_GPIO_CLK_ENABLE()	  __HAL_RCC_GPIOE_CLK_ENABLE()

#define TIM_COM_TS_ITRx                   TIM_TS_ITR3    // 内部触发配置(TIM1->ITR3->TIM4)

/* 霍尔传感器定时器 */
#define HALL_TIM           				      TIM4
#define HALL_TIM_CLK_ENABLE()  			    __HAL_RCC_TIM4_CLK_ENABLE()

extern TIM_HandleTypeDef htimx_hall;

/* 累计 TIM_Period个后产生一个更新或者中断		
	当定时器从0计数到65535，即为65535次，为一个计数周期 */
#define HALL_PERIOD_COUNT     (0xFFFF)

/* 通用控制定时器时钟源TIMxCLK = HCLK = 72MHz
	 设定定时器频率为 = TIMxCLK / (PWM_PRESCALER_COUNT) / PWM_PERIOD_COUNT = 9.987Hz
   周期 T = 100ms */
#define HALL_PRESCALER_COUNT     (110)

#define HALL_INPUT_AF_ENABLE()          	__HAL_AFIO_REMAP_TIM4_ENABLE();

/* TIM4 通道 1 引脚 */
#define HALL_INPUTU_PIN           		    GPIO_PIN_12
#define HALL_INPUTU_GPIO_PORT     		    GPIOD
#define HALL_INPUTU_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOD_CLK_ENABLE()

/* TIM4 通道 2 引脚 */
#define HALL_INPUTV_PIN           		    GPIO_PIN_13
#define HALL_INPUTV_GPIO_PORT     		    GPIOD
#define HALL_INPUTV_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOD_CLK_ENABLE()

/* TIM4 通道 3 引脚 */
#define HALL_INPUTW_PIN           		    GPIO_PIN_14
#define HALL_INPUTW_GPIO_PORT     		    GPIOD
#define HALL_INPUTW_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOD_CLK_ENABLE()

#define HALL_TIM_IRQn                    TIM4_IRQn
#define HALL_TIM_IRQHandler              TIM4_IRQHandler

extern TIM_HandleTypeDef TIM_TimeBaseStructure;

void PWM_TIMx_Configuration(void);
void stop_pwm_output(void);
void set_pwm_pulse(uint16_t pulse);
float get_motor_speed(void);
float get_motor_location(void);

void hall_enable(void);
void hall_disable(void);
void hall_tim_config(void);

#endif   /* __BSP_MOTOR_TIM_H */
