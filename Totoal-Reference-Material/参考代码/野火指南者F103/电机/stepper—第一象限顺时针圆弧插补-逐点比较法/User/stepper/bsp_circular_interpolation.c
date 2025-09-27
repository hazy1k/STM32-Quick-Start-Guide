/**
  ******************************************************************************
  * @file    bsp_stepper_liner_interpolation.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   第一象限顺时针圆弧插补-逐点比较法
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./stepper/bsp_circular_interpolation.h"
#include <stdlib.h>
#include <math.h>

Axis_TypeDef axis;
CircularInterpolation_TypeDef interpolation_para = {0};

/**
  * @brief  第一象限顺圆插补运动
  * @param  start_x：圆弧起点相对于圆心的坐标X，增量
  * @param  inc_y：终点坐标Y的增量
  * @param  speed：进给速度
  * @retval 无
  */
void Circular_InterPolation_CW(int32_t start_x, int32_t start_y, int32_t stop_x, int32_t stop_y, uint16_t speed)
{
  /* 判断当前是否正在做插补运动 */
  if(interpolation_para.motionstatus != 0)
    return;
  
  /* 检查起点、终点坐标是否在同一个圆上 */
  if(((start_x * start_x) + (start_y * start_y)) != ((stop_x * stop_x) + (stop_y * stop_y)))
    return;
  
  /* 偏差清零 */
  interpolation_para.deviation = 0;
  
  /* 起点坐标 */
  interpolation_para.startpoint_x = start_x;
  interpolation_para.startpoint_y = start_y;
  /* 终点坐标 */
  interpolation_para.endpoint_x = stop_x;
  interpolation_para.endpoint_y = stop_y;
  /* 所需脉冲数是从起点到终点的脉冲数之和 */
  interpolation_para.endpoint_pulse = abs(stop_x - start_x) + abs(stop_y - start_y);
  
  /* 第一象限正圆，x轴正转，y轴逆转 */
  interpolation_para.dir_x = CW;
  interpolation_para.dir_y = CCW;
  MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CW);
  MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CCW);
  
  /* 起点坐标x=0，说明起点在y轴上，直接向x轴进给可减小误差 */
  if(interpolation_para.startpoint_x == 0)
  {
    /* 第一步活动轴为x轴 */
    interpolation_para.active_axis = x_axis;
    /* 计算偏差 */
    interpolation_para.deviation += (2 * interpolation_para.startpoint_x + 1);
  }
  else
  {
    /* 第一步活动轴为Y轴 */
    interpolation_para.active_axis = y_axis;
    /* 计算偏差 */
    interpolation_para.deviation -= (2 * interpolation_para.startpoint_y + 1);
  }
  
  /* 设置速度 */
  __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, step_motor[x_axis].pul_channel, speed);
  __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, step_motor[y_axis].pul_channel, speed);
  __HAL_TIM_SET_AUTORELOAD(&TIM_StepperHandle, speed * 2);
  
  /* 使能主输出 */
  __HAL_TIM_MOE_ENABLE(&TIM_StepperHandle);
  /* 开启X轴比较通道输出 */
  TIM_CCxChannelCmd(MOTOR_PUL_TIM, step_motor[interpolation_para.active_axis].pul_channel, TIM_CCx_ENABLE);
  HAL_TIM_Base_Start_IT(&TIM_StepperHandle);

  interpolation_para.motionstatus = 1;
}

/**
  * @brief  定时器比较中断回调函数
  * @param  htim：定时器句柄指针
	*	@note   无
  * @retval 无
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  uint32_t last_axis = 0;
  
  /* 记录上一步的进给活动轴 */
  last_axis = interpolation_para.active_axis;
  
  /* 根据进给方向刷新坐标 */
  switch(last_axis)
  {
    case x_axis:
      switch(interpolation_para.dir_x)
      {
        case CCW: interpolation_para.startpoint_x--; break;
        case CW:  interpolation_para.startpoint_x++; break;
      }
      break;
    case y_axis:
      switch(interpolation_para.dir_y)
      {
        case CCW: interpolation_para.startpoint_y--; break;
        case CW:  interpolation_para.startpoint_y++; break;
      }
      break;
  }
  
  /* 根据上一步的偏差，判断的进给方向，并计算下一步的偏差 */
  if(interpolation_para.deviation >= 0)
  {
    /* 偏差>=0，在圆弧外侧，应向圆内进给，计算偏差 */
    interpolation_para.active_axis = y_axis;
    interpolation_para.deviation -= (2 * interpolation_para.startpoint_y + 1);
  }
  else
  {
    /* 偏差<0，在圆弧内侧，应向圆外进给，计算偏差 */
    interpolation_para.active_axis = x_axis;
    interpolation_para.deviation += (2 * interpolation_para.startpoint_x + 1);
  }
  
  /* 下一步的活动轴与上一步的不一致时，需要换轴 */
  if(last_axis != interpolation_para.active_axis)
  {
    TIM_CCxChannelCmd(htim->Instance, step_motor[last_axis].pul_channel, TIM_CCx_DISABLE);
    TIM_CCxChannelCmd(htim->Instance, step_motor[interpolation_para.active_axis].pul_channel, TIM_CCx_ENABLE);
  }
  
  /* 进给总步数减1 */
  interpolation_para.endpoint_pulse--;
  
  /* 判断是否完成插补 */
  if(interpolation_para.endpoint_pulse == 0)
  {
    /* 关闭定时器 */
    TIM_CCxChannelCmd(htim->Instance, step_motor[last_axis].pul_channel, TIM_CCx_DISABLE);
    TIM_CCxChannelCmd(htim->Instance, step_motor[interpolation_para.active_axis].pul_channel, TIM_CCx_DISABLE);
    __HAL_TIM_MOE_DISABLE(htim);
    HAL_TIM_Base_Stop_IT(htim);
    interpolation_para.motionstatus = 0;
  }
}
