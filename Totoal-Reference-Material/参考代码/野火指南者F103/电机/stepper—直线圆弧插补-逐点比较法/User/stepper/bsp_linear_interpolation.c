/**
  ******************************************************************************
  * @file    bsp_stepper_liner_interpolation.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   第一象限直线插补-逐点比较法
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./stepper/bsp_linear_interpolation.h"
#include "./stepper/bsp_circular_interpolation.h"

//Axis_TypeDef axis;
LinearInterpolation_TypeDef linear_para = {0};

/**
  * @brief  第一象限直线插补运动
  * @param  inc_x：终点坐标X的增量
  * @param  inc_y：终点坐标Y的增量
  * @param  speed：进给速度
  * @retval 无
  */
static void InterPolation_Move(uint32_t inc_x, uint32_t inc_y, uint16_t speed)
{
  /* 偏差清零 */
  linear_para.deviation = 0;
  
  /* 设置终点坐标 */
  linear_para.endpoint_x = inc_x;
  linear_para.endpoint_y = inc_y;
  /* 所需脉冲数为X、Y坐标增量之和 */
  linear_para.endpoint_pulse = inc_x + inc_y;
  
  /* 第一步进给的活动轴为X轴 */
  linear_para.active_axis = x_axis;
  /* 计算偏差 */
  linear_para.deviation -= linear_para.endpoint_y;
  
  /* 设置速度 */
  __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, step_motor[x_axis].pul_channel, speed);
  __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, step_motor[y_axis].pul_channel, speed);
  __HAL_TIM_SET_AUTORELOAD(&TIM_StepperHandle, speed * 2);
  
  /* 使能主输出 */
  __HAL_TIM_MOE_ENABLE(&TIM_StepperHandle);
  /* 开启X轴比较通道输出 */
  TIM_CCxChannelCmd(MOTOR_PUL_TIM, step_motor[linear_para.active_axis].pul_channel, TIM_CCx_ENABLE);
  HAL_TIM_Base_Start_IT(&TIM_StepperHandle);
  
  linear_para.motionstatus = 1;
}

/**
  * @brief  任意直线插补运动
  * @param  coordi_x：终点坐标X的增量
  * @param  coordi_y：终点坐标Y的增量
  * @param  speed：进给速度，定时器计数值
  * @retval 无
  */
void Linear_Interpolation(int32_t coordi_x, int32_t coordi_y, uint16_t speed)
{
  /* 设置插补模式为直线插补 */
  mode = Linear;
  
  /* 判断当前是否正在做插补运动 */
  if(linear_para.motionstatus != 0)
    return;
  
  /* 判断坐标正负，以此决定各轴的运动方向 */
  if(coordi_x < 0)
  {
    linear_para.dir_x = CCW;
    coordi_x = -coordi_x;
    MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CCW);
  }
  else
  {
    linear_para.dir_x = CW;
    MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CW);
  }
  
  if(coordi_y < 0)
  {
    linear_para.dir_y = CCW;
    coordi_y = -coordi_y;
    MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CCW);
  }
  else
  {
    linear_para.dir_y = CW;
    MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CW);
  }
  
  /* 开始插补运动 */
  InterPolation_Move(coordi_x, coordi_y, speed);
}

/**
  * @brief  直线插补处理函数
  * @param  htim：定时器句柄指针
	*	@note   定时器中断里调用
  * @retval 无
  */
void LinearInterpolation_Handler(TIM_HandleTypeDef *htim)
{
  uint32_t last_axis = 0;
  
  /* 记录上一步的进给活动轴 */
  last_axis = linear_para.active_axis;

  
  /* 根据上一步的偏差，判断的进给方向，并计算下一步的偏差 */
  if(linear_para.deviation >= 0)
  {
    /* 偏差>0，在直线上方，进给X轴，计算偏差 */
    linear_para.active_axis = x_axis;
    linear_para.deviation -= linear_para.endpoint_y;
  }
  else
  {
    /* 偏差<0，在直线下方，进给Y轴，计算偏差 */
    linear_para.active_axis = y_axis;
    linear_para.deviation += linear_para.endpoint_x;
  }
  
  /* 下一步的活动轴与上一步的不一致时，需要换轴 */
  if(last_axis != linear_para.active_axis)
  {
    TIM_CCxChannelCmd(htim->Instance, step_motor[last_axis].pul_channel, TIM_CCx_DISABLE);
    TIM_CCxChannelCmd(htim->Instance, step_motor[linear_para.active_axis].pul_channel, TIM_CCx_ENABLE);
  }
  
  /* 进给总步数减1 */
  linear_para.endpoint_pulse--;
    
  /* 判断是否完成插补 */
  if(linear_para.endpoint_pulse == 0)
  {
    /* 关闭定时器 */
    TIM_CCxChannelCmd(htim->Instance, step_motor[last_axis].pul_channel, TIM_CCx_DISABLE);
    TIM_CCxChannelCmd(htim->Instance, step_motor[linear_para.active_axis].pul_channel, TIM_CCx_DISABLE);
    __HAL_TIM_MOE_DISABLE(htim);
    HAL_TIM_Base_Stop_IT(htim);
    linear_para.motionstatus = 0;
  }
}
