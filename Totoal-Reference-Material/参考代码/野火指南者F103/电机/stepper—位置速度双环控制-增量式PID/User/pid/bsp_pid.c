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

/* 定义全局变量 */
_pid speed_pid,move_pid;
float set_point=0.0;
int pid_status=0;

/**
  * @brief  PID参数初始化
	*	@note 	无
  * @retval 无
  */
void PID_param_init()
{
	/* 初始化位置环PID参数 */
  move_pid.target_val=0.0;				
  move_pid.actual_val=0.0;
	move_pid.err = 0.0;
	move_pid.err_last = 0.0;
	move_pid.err_next = 0.0;
  move_pid.Kp = 0.0035;
  move_pid.Ki = 0.0;
  move_pid.Kd = 0.0;		
		
  #if PID_ASSISTANT_EN
    float move_pid_temp[3] = {move_pid.Kp, move_pid.Ki, move_pid.Kd};
    set_computer_value(SEED_P_I_D_CMD, CURVES_CH1, move_pid_temp, 3);// 给通道 1 发送 P I D 值
  #endif
	HAL_Delay(10);
	/* 初始化速度环PID参数 */
  speed_pid.target_val=0.0;				
  speed_pid.actual_val=0.0;
	speed_pid.err = 0.0;
	speed_pid.err_last = 0.0;
	speed_pid.err_next = 0.0;
  speed_pid.Kp = 0.05;
  speed_pid.Ki = 0.01;
  speed_pid.Kd = 0.15;

  #if PID_ASSISTANT_EN
    float speed_pid_temp[3] = {speed_pid.Kp, speed_pid.Ki, speed_pid.Kd};
    set_computer_value(SEED_P_I_D_CMD, CURVES_CH2, speed_pid_temp, 3);// 给通道 1 发送 P I D 值
  #endif
}

/**
  * @brief  设置目标值
  * @param  val		目标值
	*	@note 	无
  * @retval 无
  */
void set_pid_target(_pid *pid, float temp_val)
{
  pid->target_val = temp_val;    // 设置当前的目标值
}

/**
  * @brief  获取目标值
  * @param  无
	*	@note 	无
  * @retval 目标值
  */
float get_pid_actual(_pid *pid)
{
  return pid->target_val;    // 设置当前的目标值
}

/**
  * @brief  设置比例、积分、微分系数
  * @param  p：比例系数 P
  * @param  i：积分系数 i
  * @param  d：微分系数 d
	*	@note 	无
  * @retval 无
  */
void set_p_i_d(_pid *pid, float p, float i, float d)
{
  	pid->Kp = p;    // 设置比例系数 P
		pid->Ki = i;    // 设置积分系数 I
		pid->Kd = d;    // 设置微分系数 D
}

/**
  * @brief  增量式PID算法实现
  * @param  val：当前实际值
	*	@note 	无
  * @retval 通过PID计算后的输出
  */
float PID_realize(_pid *pid, float temp_val) 
{
	/*传入实际值*/
	pid->actual_val = temp_val;
	/*计算目标值与实际值的误差*/
  pid->err=pid->target_val-pid->actual_val;

	/*PID算法实现*/
	float increment_val = pid->Kp*(pid->err - pid->err_next) + pid->Ki*pid->err + pid->Kd*(pid->err - 2 * pid->err_next + pid->err_last);
	/*传递误差*/
	pid->err_last = pid->err_next;
	pid->err_next = pid->err;
	/*返回增量值*/
	return increment_val;
}


