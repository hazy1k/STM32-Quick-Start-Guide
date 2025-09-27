#ifndef __BSP_BLDCM_CONTROL_H
#define	__BSP_BLDCM_CONTROL_H

#include "stm32f1xx.h"
#include "./tim/bsp_motor_tim.h"
#include "main.h"

//引脚定义
/*******************************************************/
// 连接驱动板的 SD 脚
#define SHUTDOWN_PIN                  GPIO_PIN_15
#define SHUTDOWN_GPIO_PORT            GPIOD
#define SHUTDOWN_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()
/*******************************************************/

/* 电机 SD or EN 使能脚 */
#define BLDCM_ENABLE_SD()                     HAL_GPIO_WritePin(SHUTDOWN_GPIO_PORT, SHUTDOWN_PIN, GPIO_PIN_SET)      // 高电平打开-高电平使能 
#define BLDCM_DISABLE_SD()                    HAL_GPIO_WritePin(SHUTDOWN_GPIO_PORT, SHUTDOWN_PIN, GPIO_PIN_RESET)    // 低电平关断-低电平禁用

/* 电机方向控制枚举 */
typedef enum
{
  MOTOR_FWD = 0,
  MOTOR_REV,
}motor_dir_t;

typedef struct
{
  motor_dir_t direction;    // 电机方向
  uint16_t dutyfactor;      // PWM 输出占空比
  uint8_t is_enable;        // 使能电机
}bldcm_data_t;

void bldcm_init(void);
void set_bldcm_speed(uint16_t v);
void set_bldcm_direction(motor_dir_t dir);
motor_dir_t get_bldcm_direction(void);
void set_bldcm_enable(void);
void set_bldcm_disable(void);
void bldcm_pid_control(void);

#endif /* __LED_H */

