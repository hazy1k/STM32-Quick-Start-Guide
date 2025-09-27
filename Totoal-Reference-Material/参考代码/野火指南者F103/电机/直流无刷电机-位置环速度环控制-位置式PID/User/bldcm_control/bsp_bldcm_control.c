/**
  ******************************************************************************
  * @file    bsp_motor_control.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   电机控制接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include ".\bldcm_control\bsp_bldcm_control.h"
#include "./usart/bsp_debug_usart.h"
#include "./pid/bsp_pid.h"
#include <math.h>
#include <stdlib.h>

#define TARGET_SPEED_MAX    3000    // 目标速度的最大值 r/m

/* 私有变量 */
static bldcm_data_t bldcm_data;

/* 局部函数 */
static void sd_gpio_config(void);

/**
  * @brief  电机初始化
  * @param  无
  * @retval 无
  */
void bldcm_init(void)
{
  PWM_TIMx_Configuration();    // 电机控制定时器，引脚初始化
  hall_tim_config();       // 霍尔传感器初始化
  sd_gpio_config();        // sd 引脚初始化
}

/**
  * @brief  电机 SD 控制引脚初始化
  * @param  无
  * @retval 无
  */
static void sd_gpio_config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 定时器通道功能引脚端口时钟使能 */
	SHUTDOWN_GPIO_CLK_ENABLE();
  
  /* 引脚IO初始化 */
	/*设置输出类型*/
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	/*设置引脚速率 */ 
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	/*选择要控制的GPIO引脚*/	
	GPIO_InitStruct.Pin = SHUTDOWN_PIN;
  
	/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
  HAL_GPIO_Init(SHUTDOWN_GPIO_PORT, &GPIO_InitStruct);
	
	BLDCM_ENABLE_SD();     // 默认开启
	HAL_Delay(1);
}

/**
  * @brief  设置电机速度
  * @param  v: 速度（占空比）
  * @retval 无
  */
void set_bldcm_speed(uint16_t v)
{
  bldcm_data.dutyfactor = v;
  
  set_pwm_pulse(v);     // 设置速度
}

/**
  * @brief  设置电机方向
  * @param  无
  * @retval 无
  */
void set_bldcm_direction(motor_dir_t dir)
{
  bldcm_data.direction = dir;
}

/**
  * @brief  获取电机当前方向
  * @param  无
  * @retval 无
  */
motor_dir_t get_bldcm_direction(void)
{
  return bldcm_data.direction;
}

/**
  * @brief  使能电机
  * @param  无
  * @retval 无
  */
void set_bldcm_enable(void)
{
	bldcm_data.is_enable = 1;
  hall_enable();
}

/**
  * @brief  禁用电机
  * @param  无
  * @retval 无
  */
void set_bldcm_disable(void)
{
  /* 禁用霍尔传感器接口 */
  hall_disable();
  
  /* 停止 PWM 输出 */
  stop_pwm_output();
  
  /* 关闭 MOS 管 */
  bldcm_data.is_enable = 0;
}

/**
  * @brief  电机位置式 PID 控制实现(定时调用)
  * @param  无
  * @retval 无
  */
void bldcm_pid_control(void)
{
  int32_t location_actual = get_motor_location();   // 电机旋转的当前位置

	
  if (bldcm_data.is_enable)
  {
    float cont_val = 0;    // 当前控制值

		cont_val = location_pid_realize(&pid_location, location_actual);    // 进行 PID 计算
		
		/* 目标速度上限处理 */
		if (cont_val > TARGET_SPEED_MAX)
		{
			cont_val = TARGET_SPEED_MAX;
		}
		else if (cont_val < -TARGET_SPEED_MAX)
		{
			cont_val = -TARGET_SPEED_MAX;
		}
		
		set_pid_target(&pid_speed, cont_val);    // 设定位置的目标值
		
		#if defined(PID_ASSISTANT_EN)
			int32_t temp = cont_val;
			set_computer_value(SEND_TARGET_CMD, CURVES_CH2, &temp, 1);     // 给通道 2 发送目标值
		#endif
	
		
		int32_t actual_speed = get_motor_speed();
		cont_val = speed_pid_realize(&pid_speed, actual_speed);    // 进行 PID 计算
		if (cont_val < 0)
		{
				cont_val = -cont_val;
				bldcm_data.direction = MOTOR_REV;
		}
		else 
		{
				bldcm_data.direction = MOTOR_FWD;
		}

		cont_val = (cont_val > PWM_PERIOD_COUNT) ? PWM_PERIOD_COUNT : cont_val;  // 上限处理

		set_bldcm_speed(cont_val);
		
	#if defined(PID_ASSISTANT_EN)
		set_computer_value(SEND_FACT_CMD, CURVES_CH2, &actual_speed, 1);      // 给通道 2 发送实际值
		set_computer_value(SEND_FACT_CMD, CURVES_CH1, &location_actual, 1);     // 给通道 1 发送实际值
	#else
		printf("实际值：%d, 目标值： %.0f，控制值: %.0f\n", location_actual, get_pid_target(), cont_val);
	#endif
	}
	
}

