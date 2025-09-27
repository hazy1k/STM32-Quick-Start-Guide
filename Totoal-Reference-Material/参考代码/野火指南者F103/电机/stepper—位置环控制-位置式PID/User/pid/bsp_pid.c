/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   PID算法实现
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "./pid/bsp_pid.h"
#include "math.h"
#include "./stepper/bsp_stepper_ctrl.h"

/* 定义全局变量 */
_pid pid;
float set_point=0.0;
int pid_status=0;

/**
  * @brief  PID参数初始化
	*	@note 	无
  * @retval 无
  */
void PID_param_init()
{
	/* 初始化参数 */
  pid.target_val=0.0;				
  pid.actual_val=0.0;
  pid.err=0.0;
  pid.err_last=0.0;
  pid.integral=0.0;
  pid.Kp = 1.2;
  pid.Ki = 0;
  pid.Kd = 0;

  #if PID_ASSISTANT_EN
    float pid_temp[3] = {pid.Kp, pid.Ki, pid.Kd};
    set_computer_value(SEED_P_I_D_CMD, CURVES_CH1, pid_temp, 3);// 给通道 1 发送 P I D 值
  #endif
}

/**
  * @brief  设置目标值
  * @param  val		目标值
	*	@note 	无
  * @retval 无
  */
void set_pid_actual(float temp_val)
{
  pid.target_val = temp_val;    // 设置当前的目标值
}

/**
  * @brief  获取目标值
  * @param  无
	*	@note 	无
  * @retval 目标值
  */
float get_pid_actual(void)
{
  return pid.target_val;    // 设置当前的目标值
}

/**
  * @brief  设置比例、积分、微分系数
  * @param  p：比例系数 P
  * @param  i：积分系数 i
  * @param  d：微分系数 d
	*	@note 	无
  * @retval 无
  */
void set_p_i_d(float p, float i, float d)
{
  	pid.Kp = p;    // 设置比例系数 P
		pid.Ki = i;    // 设置积分系数 I
		pid.Kd = d;    // 设置微分系数 D
}

/**
  * @brief  位置式PID算法实现
  * @param  actual_val：当前实际值
	*	@note 	无
  * @retval 通过PID计算后的输出
  */
float PID_realize(float actual_val) 
{
  /*传入实际值*/
  pid.actual_val = actual_val;
  /*计算目标值与实际值的误差*/
  pid.err = pid.target_val - pid.actual_val;

  /*误差累积*/
  pid.integral += pid.err;
  /*PID算法实现*/
  pid.actual_val = pid.Kp*pid.err + pid.Ki*pid.integral + pid.Kd*(pid.err-pid.err_last);
  /*误差传递*/
  pid.err_last = pid.err;
  /*PID算法实现，并返回计算值*/
  return pid.actual_val;
}

