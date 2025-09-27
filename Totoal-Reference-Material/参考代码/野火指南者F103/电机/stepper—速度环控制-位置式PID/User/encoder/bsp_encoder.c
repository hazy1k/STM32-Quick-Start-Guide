/**
  ******************************************************************************
  * @file    bsp_motor_control.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   编码器接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "./tim/bsp_basic_tim.h"
#include "./stepper/bsp_stepper_ctrl.h"
#include "./Encoder/bsp_encoder.h"

/* 定时器溢出次数 */
__IO int16_t Encoder_Overflow_Count = 0;

TIM_HandleTypeDef TIM_EncoderHandle;

/**
  * @brief  编码器接口引脚初始化
  * @param  无
  * @retval 无
  */
static void Encoder_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  ENCODER_TIM_CLK_ENABLE();
  /* 定时器通道引脚端口时钟使能 */
  ENCODER_TIM_CH1_GPIO_CLK_ENABLE();
  ENCODER_TIM_CH2_GPIO_CLK_ENABLE();
  
  /**TIM3 GPIO Configuration    
    PA6     ------> TIM3_CH1
    PA7     ------> TIM3_CH2
  */
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
  
  /* 选择要控制的GPIO引脚 */	
  GPIO_InitStruct.Pin = ENCODER_TIM_CH1_PIN;
  /* 调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO */
  HAL_GPIO_Init(ENCODER_TIM_CH1_GPIO_PORT, &GPIO_InitStruct);
  
  /* 选择要控制的GPIO引脚 */	
  GPIO_InitStruct.Pin = ENCODER_TIM_CH2_PIN;
  /* 调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO */
  HAL_GPIO_Init(ENCODER_TIM_CH2_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  配置TIMx编码器模式
  * @param  无
  * @retval 无
  */
static void TIM_Encoder_Init(void)
{ 
  TIM_Encoder_InitTypeDef Encoder_ConfigStructure;
  
  /* 使能编码器接口时钟 */
  ENCODER_TIM_CLK_ENABLE();
  
  /* 定时器初始化设置 */
  TIM_EncoderHandle.Instance = ENCODER_TIM;
  TIM_EncoderHandle.Init.Prescaler = ENCODER_TIM_PRESCALER;
  TIM_EncoderHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TIM_EncoderHandle.Init.Period = ENCODER_TIM_PERIOD;
  TIM_EncoderHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  TIM_EncoderHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  
  /* 设置编码器倍频数 */
  Encoder_ConfigStructure.EncoderMode = ENCODER_MODE;
  /* 编码器接口通道1设置 */
  Encoder_ConfigStructure.IC1Polarity = ENCODER_IC1_POLARITY;
  Encoder_ConfigStructure.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  Encoder_ConfigStructure.IC1Prescaler = TIM_ICPSC_DIV1;
  Encoder_ConfigStructure.IC1Filter = 0;
  /* 编码器接口通道2设置 */
  Encoder_ConfigStructure.IC2Polarity = ENCODER_IC2_POLARITY;
  Encoder_ConfigStructure.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  Encoder_ConfigStructure.IC2Prescaler = TIM_ICPSC_DIV1;
  Encoder_ConfigStructure.IC2Filter = 0;
  /* 初始化编码器接口 */
  HAL_TIM_Encoder_Init(&TIM_EncoderHandle, &Encoder_ConfigStructure);
  
  /* 清零计数器 */
  __HAL_TIM_SET_COUNTER(&TIM_EncoderHandle, 0);
  
  /* 清零中断标志位 */
  __HAL_TIM_CLEAR_IT(&TIM_EncoderHandle,TIM_IT_UPDATE);
  /* 使能定时器的更新事件中断 */
  __HAL_TIM_ENABLE_IT(&TIM_EncoderHandle,TIM_IT_UPDATE);
  /* 设置更新事件请求源为：计数器溢出 */
  __HAL_TIM_URS_ENABLE(&TIM_EncoderHandle);
  
  /* 设置中断优先级 */
  HAL_NVIC_SetPriority(ENCODER_TIM_IRQn, 5, 1);
  /* 使能定时器中断 */
  HAL_NVIC_EnableIRQ(ENCODER_TIM_IRQn);
  
  /* 使能编码器接口 */
  HAL_TIM_Encoder_Start(&TIM_EncoderHandle, TIM_CHANNEL_ALL);
}

/**
  * @brief  编码器接口初始化
  * @param  无
  * @retval 无
  */
void Encoder_Init(void)
{
  Encoder_GPIO_Init();    /* 引脚初始化 */
  TIM_Encoder_Init();     /* 配置编码器接口 */
}

/**
  * @brief  定时器更新事件回调函数
  * @param  无
  * @retval 无
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* 判断触发中断的定时器 */
  if(htim->Instance == BASIC_TIM)
  {
    Stepper_Speed_Ctrl();
  }
  else if(htim->Instance == ENCODER_TIM)
  {  
    /* 判断当前计数方向 */
    if(__HAL_TIM_IS_TIM_COUNTING_DOWN(htim))
      /* 下溢 */
      Encoder_Overflow_Count--;
    else
      /* 上溢 */
      Encoder_Overflow_Count++;
  }
}


/*********************************************END OF FILE**********************/

