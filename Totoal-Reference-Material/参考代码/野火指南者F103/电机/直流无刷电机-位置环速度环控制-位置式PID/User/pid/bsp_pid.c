#include "./pid/bsp_pid.h"
#include ".\bldcm_control\bsp_bldcm_control.h"

//定义全局变量

_pid pid_location;
_pid pid_speed;

/**
  * @brief  PID参数初始化
	*	@note 	无
  * @retval 无
  */
void PID_param_init()
{
	/* 位置相关初始化参数 */
  pid_location.target_val=0.0;				
  pid_location.actual_val=0.0;
  pid_location.err=0.0;
  pid_location.err_last=0.0;
  pid_location.integral=0.0;

  pid_location.Kp = 104;
  pid_location.Ki = 0;
  pid_location.Kd = 120;
	
	/* 速度相关初始化参数 */
	pid_speed.target_val=0.0;				
	pid_speed.actual_val=0.0;
	pid_speed.err=0.0;
	pid_speed.err_last=0.0;
	pid_speed.integral=0.0;

	pid_speed.Kp = 0.062;
	pid_speed.Ki = 0.011;
	pid_speed.Kd = 0.0;


#if defined(PID_ASSISTANT_EN)
  float pid_temp[3] = {pid_location.Kp, pid_location.Ki, pid_location.Kd};
  set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, pid_temp, 3);     // 给通道 1 发送 P I D 值
	pid_temp[0] = pid_speed.Kp;
	pid_temp[1] = pid_speed.Ki;
	pid_temp[2] = pid_speed.Kd;
  set_computer_value(SEND_P_I_D_CMD, CURVES_CH2, pid_temp, 3);     // 给通道 2 发送 P I D 值
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
float get_pid_target(_pid *pid)
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
  * @brief  位置PID算法实现
  * @param  actual_val:实际值
	*	@note 	无
  * @retval 通过PID计算后的输出
  */
float location_pid_realize(_pid *pid, float actual_val)
{
		/*计算目标值与实际值的误差*/
    pid->err=pid->target_val-actual_val;
    pid->integral += pid->err;    // 误差累积

	  if((pid->err<1.0f )&& (pid->err>-1.0f))
		{
			pid->err = 0.0f;
		}	

		/*PID算法实现*/
    pid->actual_val = pid->Kp*pid->err+pid->Ki*pid->integral+pid->Kd*(pid->err-pid->err_last);
  
		/*误差传递*/
    pid->err_last=pid->err;
    
		/*返回当前实际值*/
    return pid->actual_val;
}

/**
  * @brief  速度PID算法实现
  * @param  actual_val:实际值
	*	@note 	无
  * @retval 通过PID计算后的输出
  */
float speed_pid_realize(_pid *pid, float actual_val)
{
		/*计算目标值与实际值的误差*/
    pid->err=pid->target_val-actual_val;

    if((pid->err<41.5f )&& (pid->err>-41.5f))
      pid->err = 0.0f;
		

    pid->integral += pid->err;    // 误差累积

		/*PID算法实现*/
    pid->actual_val = pid->Kp*pid->err+pid->Ki*pid->integral+pid->Kd*(pid->err-pid->err_last);
  
		/*误差传递*/
    pid->err_last=pid->err;
    
		/*返回当前实际值*/
    return pid->actual_val;
}




