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

#include ".\motor_control\bsp_motor_control.h"
#include "./usart/bsp_debug_usart.h"
#include <math.h>
#include <stdlib.h>
#include "./Encoder/bsp_encoder.h"
#include "./adc/bsp_adc.h"
#include "./tim/bsp_basic_tim.h"

static motor_dir_t direction  = MOTOR_FWD;     // 记录方向
static uint16_t    dutyfactor = 0;             // 记录占空比
static uint8_t    is_motor_en = 0;             // 电机使能

#define TARGET_CURRENT_MAX    130    // 目标电流的最大值 mA

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
}

/**
  * @brief  电机初始化
  * @param  无
  * @retval 无
  */
void motor_init(void)
{
  Motor_TIMx_Configuration();     // 初始化电机 1
  sd_gpio_config();
}

/**
  * @brief  设置电机速度
  * @param  v: 速度（占空比）
  * @retval 无
  */
void set_motor_speed(uint16_t v)
{
  v = (v > PWM_PERIOD_COUNT) ? PWM_PERIOD_COUNT : v;     // 上限处理
  
  dutyfactor = v;
  
  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // 设置速度
  }
  else
  {
    SET_REV_COMPAER(dutyfactor);     // 设置速度
  }
}

/**
  * @brief  设置电机方向
  * @param  无
  * @retval 无
  */
void set_motor_direction(motor_dir_t dir)
{
  direction = dir;

  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // 设置速度
    SET_REV_COMPAER(0);              // 设置速度
  }
  else
  {
    SET_FWD_COMPAER(0);              // 设置速度
    SET_REV_COMPAER(dutyfactor);     // 设置速度
  }
}

/**
  * @brief  使能电机
  * @param  无
  * @retval 无
  */
void set_motor_enable(void)
{
  is_motor_en = 1;	
  MOTOR_ENABLE_SD();
  MOTOR_FWD_ENABLE();
  MOTOR_REV_ENABLE();
}

/**
  * @brief  禁用电机
  * @param  无
  * @retval 无
  */
void set_motor_disable(void)
{
  is_motor_en = 0;
  MOTOR_DISABLE_SD();
  MOTOR_FWD_DISABLE();
  MOTOR_REV_DISABLE();
}

/**
  * @brief  电机位置式 PID 控制实现(定时调用)
  * @param  无
  * @retval 无
  */
void motor_pid_control(void)
{
  static uint32_t location_timer = 0;         // 位置环周期
  int32_t actual_current = get_curr_val();    // 读取当前电流值
  
  if (is_motor_en == 1)                  // 电机在使能状态下才进行控制处理
  {
    static int32_t Capture_Count = 0;    // 当前时刻总计数值
    float cont_val = 0;                  // 当前控制值
    
    /* 当前时刻总计数值 = 计数器值 + 计数溢出次数 * ENCODER_TIM_PERIOD  */
    Capture_Count = __HAL_TIM_GET_COUNTER(&TIM_EncoderHandle) + (Encoder_Overflow_Count * ENCODER_TIM_PERIOD);
    
    if (location_timer++%2 == 0)
    {
      cont_val = location_pid_realize(&pid_location, Capture_Count);    // 进行 PID 计算

      if (cont_val > 0)    // 判断电机方向
      {
        set_motor_direction(MOTOR_FWD);
      }
      else
      {
        cont_val = -cont_val;
        set_motor_direction(MOTOR_REV);
      }
      
      cont_val = (cont_val > TARGET_CURRENT_MAX) ? TARGET_CURRENT_MAX : cont_val;    // 电流上限处理
      set_pid_target(&pid_curr, cont_val);    // 设定速度的目标值
      
    #if defined(PID_ASSISTANT_EN)
      int32_t temp = cont_val;
      set_computer_value(SEND_TARGET_CMD, CURVES_CH2, &temp, 1);     // 给通道 2 发送目标值
    #endif
    }
    
    cont_val = curr_pid_realize(&pid_curr, actual_current);    // 进行 PID 计算
    
    if (cont_val < 0)
    {
      cont_val = 0;    // 下限处理
    }
    else if (cont_val > PWM_MAX_PERIOD_COUNT)
    {
      cont_val = PWM_MAX_PERIOD_COUNT;    // 速度上限处理
    }

    set_motor_speed(cont_val);                                                 // 设置 PWM 占空比
    
  #if defined(PID_ASSISTANT_EN)
    set_computer_value(SEND_FACT_CMD, CURVES_CH1, &Capture_Count,  1);          // 给通道 1 发送实际值
//    set_computer_value(SEND_FACT_CMD, CURVES_CH2, &actual_current, 1);         // 给通道 2 发送实际值
  #else
    printf("实际值：%d. 目标值：%.0f. 差值：%.0f. 控制值：%.0f\n", Capture_Count, get_pid_target(&pid_location), (float)Capture_Count - get_pid_target(&pid_location), cont_val);      // 打印实际值和目标值
  #endif
  }
}

/*********************************************END OF FILE**********************/

