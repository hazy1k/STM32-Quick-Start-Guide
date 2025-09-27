/**
  ******************************************************************************
  * @file    bsp_advance_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   高级控制定时器定时范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_advance_tim.h"

TIM_HandleTypeDef TIM_AdvanceHandle;

__IO uint16_t OC_Pulse_num_Channel1 = 25;    /* 通道1的比较值 */
__IO uint16_t OC_Pulse_num_Channel2 = 51;    /* 通道2的比较值 */
__IO uint16_t OC_Pulse_num_Channel3 = 77;    /* 通道3的比较值 */
__IO uint16_t OC_Pulse_num_Channel4 = 103;   /* 通道4的比较值 */

/**
  * @brief  高级控制定时器 TIMx,x[1,8]中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIMx_NVIC_Configuration(void)
{
	/* 设置抢占优先级，子优先级 */
	HAL_NVIC_SetPriority(ADVANCE_TIM_IRQn, 0, 0);
	/* 设置中断源 */
	HAL_NVIC_EnableIRQ(ADVANCE_TIM_IRQn);
}

/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIMx_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启定时器相关的GPIO外设时钟*/
	CHANNEL1_OC_GPIO_CLK_ENABLE();
	CHANNEL2_OC_GPIO_CLK_ENABLE();
  CHANNEL3_OC_GPIO_CLK_ENABLE();
  CHANNEL4_OC_GPIO_CLK_ENABLE();

	/* 定时器功能引脚初始化 */															   
	GPIO_InitStructure.Pin = CHANNEL1_OC_PIN;	
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;    
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; 	
	HAL_GPIO_Init(CHANNEL1_OC_GPIO_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.Pin = CHANNEL2_OC_PIN;	
	HAL_GPIO_Init(CHANNEL2_OC_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.Pin = CHANNEL3_OC_PIN;	
	HAL_GPIO_Init(CHANNEL3_OC_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.Pin = CHANNEL4_OC_PIN;	
	HAL_GPIO_Init(CHANNEL4_OC_GPIO_PORT, &GPIO_InitStructure);
}

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
static void TIM_Mode_Config(void)
{
	TIM_OC_InitTypeDef  TIM_OCInitStructure;
  
	/*使能定时器*/
	ADVANCE_TIM_CLK_ENABLE();

	TIM_AdvanceHandle.Instance = ADVANCE_TIM;    
	/* 累计 TIM_Period个后产生一个更新或者中断 */		
	//当定时器从0计数到TIM_PERIOD，即为TIM_PERIOD次，为一个定时周期
	TIM_AdvanceHandle.Init.Period = TIM_PERIOD; 
	// 高级控制定时器时钟源TIMxCLK = HCLK=168MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_PRESCALER-1)
	TIM_AdvanceHandle.Init.Prescaler = TIM_PRESCALER-1;
	/* 计数方式 */
	TIM_AdvanceHandle.Init.CounterMode = TIM_COUNTERMODE_UP;            
	/* 采样时钟分频 */	
	TIM_AdvanceHandle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;   
	TIM_AdvanceHandle.Init.RepetitionCounter = 0 ;  		
	/* 初始化定时器 */
	HAL_TIM_OC_Init(&TIM_AdvanceHandle);

	/* PWM模式配置--这里配置为输出比较模式 */
	TIM_OCInitStructure.OCMode = TIM_OCMODE_TOGGLE; 
	/* 比较输出的计数值 */
	TIM_OCInitStructure.Pulse = OC_Pulse_num_Channel1;
	/* 当定时器计数值小于CCR1_Val时为高电平 */
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
	/* 设置互补通道输出的极性 */
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_LOW; 
	/* 快速模式设置 */
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;   
	/* 空闲电平 */
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_RESET;  
	/* 互补通道设置 */
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET; 
	HAL_TIM_OC_ConfigChannel(&TIM_AdvanceHandle, &TIM_OCInitStructure, ADVANCE_TIM_CHANNEL_1);
  
  /* 通道2配置 */
  TIM_OCInitStructure.Pulse = OC_Pulse_num_Channel2;
  HAL_TIM_OC_ConfigChannel(&TIM_AdvanceHandle, &TIM_OCInitStructure, ADVANCE_TIM_CHANNEL_2);
    
  /* 通道3配置 */
  TIM_OCInitStructure.Pulse = OC_Pulse_num_Channel3;
  HAL_TIM_OC_ConfigChannel(&TIM_AdvanceHandle, &TIM_OCInitStructure, ADVANCE_TIM_CHANNEL_3);
  
  /* 通道4配置 */
  TIM_OCInitStructure.Pulse = OC_Pulse_num_Channel4;
  HAL_TIM_OC_ConfigChannel(&TIM_AdvanceHandle, &TIM_OCInitStructure, ADVANCE_TIM_CHANNEL_4);

	/* 启动比较输出并使能中断 */
	HAL_TIM_OC_Start_IT(&TIM_AdvanceHandle, ADVANCE_TIM_CHANNEL_1);
  /* 启动比较输出并使能中断 */
	HAL_TIM_OC_Start_IT(&TIM_AdvanceHandle, ADVANCE_TIM_CHANNEL_2);
  /* 启动比较输出并使能中断 */
	HAL_TIM_OC_Start_IT(&TIM_AdvanceHandle, ADVANCE_TIM_CHANNEL_3);
  /* 启动比较输出并使能中断 */
	HAL_TIM_OC_Start_IT(&TIM_AdvanceHandle, ADVANCE_TIM_CHANNEL_4);
  
	/* 使能比较通道1 */
	TIM_CCxChannelCmd(ADVANCE_TIM, ADVANCE_TIM_CHANNEL_1, TIM_CCx_ENABLE);
  /* 使能比较通道2 */
	TIM_CCxChannelCmd(ADVANCE_TIM, ADVANCE_TIM_CHANNEL_2, TIM_CCx_ENABLE);
  /* 使能比较通道3 */
	TIM_CCxChannelCmd(ADVANCE_TIM, ADVANCE_TIM_CHANNEL_3, TIM_CCx_ENABLE);
  /* 使能比较通道4 */
	TIM_CCxChannelCmd(ADVANCE_TIM, ADVANCE_TIM_CHANNEL_4, TIM_CCx_ENABLE);
}

/**
  * @brief  初始化高级控制定时器外设
  * @param  无
  * @retval 无
  */
void TIMx_AdvanceConfig(void)
{
	TIMx_GPIO_Config();	

	TIM_Mode_Config();
  
  TIMx_NVIC_Configuration();
}


/**
  * @brief  定时器比较中断
  * @param  htim：定时器句柄指针
	*	@note 	无
  * @retval 无
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  __IO uint16_t count;
  
  /*获取当前计数*/
  count = __HAL_TIM_GET_COUNTER(htim);
  
  /*判断触发中断的输出通道并设置新的比较数值*/
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
    __HAL_TIM_SET_COMPARE(htim, ADVANCE_TIM_CHANNEL_1, count + OC_Pulse_num_Channel1);
  }
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    __HAL_TIM_SET_COMPARE(htim, ADVANCE_TIM_CHANNEL_2, count + OC_Pulse_num_Channel2);
  }
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
  {
    __HAL_TIM_SET_COMPARE(htim, ADVANCE_TIM_CHANNEL_3, count + OC_Pulse_num_Channel3);
  }
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
  {
    __HAL_TIM_SET_COMPARE(htim, ADVANCE_TIM_CHANNEL_4, count + OC_Pulse_num_Channel4);
  }
}

/*********************************************END OF FILE**********************/
