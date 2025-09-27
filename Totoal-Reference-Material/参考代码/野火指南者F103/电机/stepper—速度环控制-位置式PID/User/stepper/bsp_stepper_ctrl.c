/**
  ******************************************************************************
  * @file    bsp_stepper_init.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   步进电机初始化
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "math.h"
#include "./tim/bsp_basic_tim.h"
#include "./stepper/bsp_stepper_ctrl.h"
#include "./pid/bsp_pid.h"

extern _pid pid;
extern __IO uint16_t OC_Pulse_num;     //比较输出的计数值

__SYS_STATUS sys_status = {0};

/**
  * @brief  驱动器紧急停止
  * @param  NewState：使能或者禁止
  * @retval 无
  */
void MSD_ENA(int NewState)
{
    if(NewState)
    {
      //ENA失能，禁止驱动器输出，（脱机状态）此时电机为无保持力矩状态，可以手动旋转电机
      MOTOR_EN(OFF);
      sys_status.MSD_ENA = 0;			

    }
    else
    {
      //ENA使能，此时电机为保持力矩状态
      MOTOR_EN(ON);
      sys_status.MSD_ENA = 1;			
    }
}

/**
  * @brief  步进电机刹车
  * @param  无
  * @retval 无
  */
void Set_Stepper_Stop(void)
{
  /*失能比较通道*/
	TIM_CCxChannelCmd(MOTOR_PUL_TIM,MOTOR_PUL_CHANNEL_x,TIM_CCx_DISABLE);
  sys_status.stepper_running = 0;
}

/**
  * @brief  启动步进电机
  * @param  无
  * @retval 无
  */
void Set_Stepper_Start(void)
{
  /*使能驱动器*/
  MSD_ENA(0);
  /*使能比较通道输出*/
	TIM_CCxChannelCmd(MOTOR_PUL_TIM,MOTOR_PUL_CHANNEL_x,TIM_CCx_ENABLE);
  sys_status.MSD_ENA = 1;
  sys_status.stepper_running = 1;
}

/**
  * @brief  步进电机位置式PID控制
  * @retval 无
  * @note   基本定时器中断内调用
  */
void Stepper_Speed_Ctrl(void)
{
  /* 编码器相关变量 */
  __IO int16_t capture_per_unit = 0;
  __IO int32_t capture_count = 0;
  static __IO int32_t last_count = 0;
  
  /* 经过pid计算后的期望值 */
  __IO float cont_val = 0;

  /* 当电机运动时才启动pid计算 */
  if((sys_status.MSD_ENA == 1) && (sys_status.stepper_running == 1))
  {
    /* 计算单个采样时间内的编码器脉冲数 */
    capture_count = __HAL_TIM_GET_COUNTER(&TIM_EncoderHandle) + (Encoder_Overflow_Count * ENCODER_TIM_PERIOD);
    capture_per_unit = capture_count - last_count;
    last_count = capture_count;
    
    /* 单位时间内的编码器脉冲数作为实际值传入pid控制器 */
    cont_val = PID_realize((float)capture_per_unit);// 进行 PID 计算
    
    /* 判断速度方向 */
    cont_val > 0 ? (MOTOR_DIR(CW)) : (MOTOR_DIR(CCW));

    /* 对计算得出的期望值取绝对值 */
    cont_val = fabsf(cont_val);
    /* 计算比较计数器的值 */
    OC_Pulse_num = ((uint16_t)(T1_FREQ / (cont_val * PULSE_RATIO * SAMPLING_PERIOD))) >> 1;

   #if PID_ASSISTANT_EN
    int temp = capture_per_unit;
    set_computer_value(SEED_FACT_CMD, CURVES_CH1, &temp, 1);  // 给通道 1 发送实际值
   #else
    printf("实际值：%d，目标值：%.0f\r\n", capture_per_unit, pid.target_val);// 打印实际值和目标值
   #endif
  }
  else
  {
    capture_per_unit = 0;
    cont_val = 0;
    pid.actual_val = 0;
    pid.err = 0;
    pid.err_last = 0;
    pid.integral = 0;
  }
}
