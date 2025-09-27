#ifndef __BSP_MOTOR_CONTROL_H
#define	__BSP_MOTOR_CONTROL_H

#include "stm32f1xx.h"
#include "./tim/bsp_motor_tim.h"
#include "main.h"

/* 电机使能脚 */
#define ENA_PIN                  GPIO_PIN_12
#define ENA_GPIO_PORT            GPIOB
#define ENA_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()

#define ENB_PIN                  GPIO_PIN_13
#define ENB_GPIO_PORT            GPIOB
#define ENB_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()

/* 电机方向控制枚举 */
typedef enum
{
  MOTOR_FWD = 0,
  MOTOR_REV,
}motor_dir_t;

/* 设置速度（占空比） */
#define SET_FWD_COMPAER(ChannelPulse)        TIM1_SetPWM_pulse(PWM_CHANNEL_1,ChannelPulse)    // 设置比较寄存器的值
#define SET_REV_COMPAER(ChannelPulse)        TIM1_SetPWM_pulse(PWM_CHANNEL_2,ChannelPulse)    // 设置比较寄存器的值

/* 使能输出 */
#define MOTOR_FWD_ENABLE()                   HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,PWM_CHANNEL_1);    // 使能 PWM 通道 1
#define MOTOR_REV_ENABLE()                   HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,PWM_CHANNEL_2);    // 使能 PWM 通道 2

/* 禁用输出 */
#define MOTOR_FWD_DISABLE()                  HAL_TIM_PWM_Stop(&TIM_TimeBaseStructure,PWM_CHANNEL_1);     // 禁用 PWM 通道 1
#define MOTOR_REV_DISABLE()                  HAL_TIM_PWM_Stop(&TIM_TimeBaseStructure,PWM_CHANNEL_2);     // 禁用 PWM 通道 2

/* 电机使能脚 */
#define MOTOR_ENABLE_A()                     HAL_GPIO_WritePin(ENA_GPIO_PORT, ENA_PIN, GPIO_PIN_SET)
#define MOTOR_DISABLE_A()                    HAL_GPIO_WritePin(ENA_GPIO_PORT, ENA_PIN, GPIO_PIN_RESET)

/* 设置速度（占空比）2 */
#define SET2_FWD_COMPAER(ChannelPulse)       TIM1_SetPWM2_pulse(PWM_CHANNEL_1,ChannelPulse)    // 设置比较寄存器的值
#define SET2_REV_COMPAER(ChannelPulse)       TIM1_SetPWM2_pulse(PWM_CHANNEL_2,ChannelPulse)    // 设置比较寄存器的值

/* 使能输出2 */
#define MOTOR2_FWD_ENABLE()                  HAL_TIM_PWM_Start(&TIM_TimeBaseStructure2,PWM_CHANNEL_1);    // 使能 PWM 通道 1
#define MOTOR2_REV_ENABLE()                  HAL_TIM_PWM_Start(&TIM_TimeBaseStructure2,PWM_CHANNEL_2);    // 使能 PWM 通道 2

/* 禁用输出2 */
#define MOTOR2_FWD_DISABLE()                 HAL_TIM_PWM_Stop(&TIM_TimeBaseStructure2,PWM_CHANNEL_1);     // 禁用 PWM 通道 1
#define MOTOR2_REV_DISABLE()                 HAL_TIM_PWM_Stop(&TIM_TimeBaseStructure2,PWM_CHANNEL_2);     // 禁用 PWM 通道 2

/* 电机2使能脚 */
#define MOTOR_ENABLE_B()                     HAL_GPIO_WritePin(ENB_GPIO_PORT, ENB_PIN, GPIO_PIN_SET)
#define MOTOR_DISABLE_B()                    HAL_GPIO_WritePin(ENB_GPIO_PORT, ENB_PIN, GPIO_PIN_RESET)

void motor_init(void);
void set_motor_speed(uint16_t v);
void set_motor_direction(motor_dir_t dir);
void set_motor_enable(void);
void set_motor2_speed(uint16_t v);
void set_motor2_direction(motor_dir_t dir);
void set_motor2_enable(void);
void show_help(void);
void deal_serial_data(void);

#endif /* __LED_H */

