#ifndef __BSP_PID_H
#define	__BSP_PID_H
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
  float target_val;     //目标值
	float actual_val;     //实际值
	float err;            //定义当前偏差值
	float err_next;       //定义下一个偏差值
	float err_last;       //定义最后一个偏差值
	float Kp, Ki, Kd;     //定义比例、积分、微分系数
}_pid;

extern void PID_param_init(void);
extern void set_pid_target(float temp_val);
extern float get_pid_target(void);
extern void set_p_i_d(float p, float i, float d);
extern float PID_realize(float actual_val);
extern void time_period_fun(void);

#endif
