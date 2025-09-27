#include "./pid/bsp_pid.h"
#include "math.h"
#include "./key/bsp_key.h" 
#include "./protocol/protocol.h"

//定义全局变量

_pid pid;

/**
  * @brief  PID参数初始化
	*	@note 	无
  * @retval 无
  */
void PID_param_init()
{
		/* 初始化参数 */
//    printf("PID_init begin \n");
    pid.target_val=0.0;				
    pid.actual_val=0.0;
    pid.err=0.0;
    pid.err_last=0.0;
    pid.integral=0.0;
//		pid.Kp = 0.31;
//		pid.Ki = 0.070;
//		pid.Kd = 0.3;
//		pid.Kp = 0.21;
//		pid.Ki = 0.070;
//		pid.Kd = 0.3;
		pid.Kp = 0.01;//24
		pid.Ki = 0.80;
		pid.Kd = 0.04;

  #if defined(PID_ASSISTANT_EN)
    float pid_temp[3] = {pid.Kp, pid.Ki, pid.Kd};
    set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, pid_temp, 3);     // 给通道 1 发送 P I D 值
  #endif

}

/**
  * @brief  设置目标值
  * @param  val		目标值
	*	@note 	无
  * @retval 无
  */
void set_pid_target(float temp_val)
{
  pid.target_val = temp_val;    // 设置当前的目标值
}

/**
  * @brief  获取目标值
  * @param  无
	*	@note 	无
  * @retval 目标值
  */
float get_pid_target(void)
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
  * @brief  PID算法实现
  * @param  val		实际值
  * @note 	无
  * @retval 通过PID计算后的输出
  */
float PID_realize(float temp_val)
{
	/*计算目标值与实际值的误差*/
    pid.err=pid.target_val-temp_val;
	/*误差累积*/
    pid.integral+=pid.err;
	/*PID算法实现*/
    pid.actual_val=pid.Kp*pid.err+pid.Ki*pid.integral+pid.Kd*(pid.err-pid.err_last);
	/*误差传递*/
    pid.err_last=pid.err;
	/*返回当前实际值*/
    return pid.actual_val;
}
/**
  * @brief  定时器周期调用函数
  * @param  无
	*@note 	无
  * @retval 无
  */
float set_point=0.0;
int pid_status=0;
void time_period_fun()
{
	static int num=0;
	static int run_i=0;
			
	if(!pid_status)
	{
		float val=PID_realize(pid.actual_val);
    
    int temp = val;    // 上位机需要整数参数，转换一下
    set_computer_value(SEND_FACT_CMD, CURVES_CH1, &temp, 1);                // 给通道 1 发送实际值
//		printf("val,%f;act,%f\n",set_point,val);	
	}
}







