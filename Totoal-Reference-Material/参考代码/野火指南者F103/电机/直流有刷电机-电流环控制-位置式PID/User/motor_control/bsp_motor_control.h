#ifndef __BSP_MOTOR_CONTROL_H
#define	__BSP_MOTOR_CONTROL_H

#include "stm32f1xx.h"
#include "./tim/bsp_motor_tim.h"
#include "main.h"
#include "./pid/bsp_pid.h"

//引脚定义
/*******************************************************/
// 连接MOS管搭建板的 SD 脚，或者连接L298N板的 EN 脚
#define SHUTDOWN_PIN                  GPIO_PIN_12
#define SHUTDOWN_GPIO_PORT            GPIOB
#define SHUTDOWN_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
/*******************************************************/

/* 电机 SD or EN 使能脚 */
#define MOTOR_ENABLE_SD()                     HAL_GPIO_WritePin(SHUTDOWN_GPIO_PORT, SHUTDOWN_PIN, GPIO_PIN_SET)      // 高电平打开-高电平使能 
#define MOTOR_DISABLE_SD()                    HAL_GPIO_WritePin(SHUTDOWN_GPIO_PORT, SHUTDOWN_PIN, GPIO_PIN_RESET)    // 低电平关断-低电平禁用

/* 电机方向控制枚举 */
typedef enum
{
  MOTOR_FWD = 0,
  MOTOR_REV,
}motor_dir_t;

/* 设置速度（占空比） */
#define SET_FWD_COMPAER(ChannelPulse)     TIM1_SetPWM_pulse(PWM_CHANNEL_1,ChannelPulse)    // 设置比较寄存器的值
#define SET_REV_COMPAER(ChannelPulse)     TIM1_SetPWM_pulse(PWM_CHANNEL_2,ChannelPulse)    // 设置比较寄存器的值

/* 使能输出 */
#define MOTOR_FWD_ENABLE()      HAL_TIM_PWM_Start(&DCM_TimeBaseStructure,PWM_CHANNEL_1);
#define MOTOR_REV_ENABLE()      HAL_TIM_PWM_Start(&DCM_TimeBaseStructure,PWM_CHANNEL_2);

/* 禁用输出 */
#define MOTOR_FWD_DISABLE()     HAL_TIM_PWM_Stop(&DCM_TimeBaseStructure,PWM_CHANNEL_1);
#define MOTOR_REV_DISABLE()     HAL_TIM_PWM_Stop(&DCM_TimeBaseStructure,PWM_CHANNEL_2);

void motor_init(void);
void set_motor_speed(uint16_t v);
void set_motor_direction(motor_dir_t dir);
void set_motor_enable(void);
void set_motor_disable(void);
void show_help(void);
void deal_serial_data(void);
void motor_pid_control(void);

#endif /* __LED_H */

