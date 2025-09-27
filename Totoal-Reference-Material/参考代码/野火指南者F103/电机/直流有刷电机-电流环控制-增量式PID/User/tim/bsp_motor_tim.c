/**
  ******************************************************************************
  * @file    bsp_motor_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   PWM输出范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_motor_tim.h"

void TIM_SetTIMxCompare(TIM_TypeDef *TIMx,uint32_t channel,uint32_t compare);
void TIM_SetPWM_period(TIM_TypeDef* TIMx,uint32_t TIM_period);

/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIMx_GPIO_Config(void) 
{
 GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 定时器通道功能引脚端口时钟使能 */
	
	PWM_TIM_CH1_GPIO_CLK();
	PWM_TIM_CH2_GPIO_CLK();
  
  /* 定时器通道1功能引脚IO初始化 */
	/*设置输出类型*/
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	/*设置引脚速率 */ 
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	/*设置复用*/
	PWM_TIM_GPIO_AF_ENALBE();
	
	/*选择要控制的GPIO引脚*/	
	GPIO_InitStruct.Pin = PWM_TIM_CH1_PIN;
	/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
  HAL_GPIO_Init(PWM_TIM_CH1_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PWM_TIM_CH2_PIN;	
  HAL_GPIO_Init(PWM_TIM_CH2_GPIO_PORT, &GPIO_InitStruct);
	
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
TIM_HandleTypeDef  DCM_TimeBaseStructure;
static void TIM_PWMOUTPUT_Config(void)
{
  TIM_OC_InitTypeDef  TIM_OCInitStructure;  
	
  /*使能定时器*/
  PWM_TIM_CLK_ENABLE();
	
  DCM_TimeBaseStructure.Instance = PWM_TIM;
  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到PWM_PERIOD_COUNT，即为PWM_PERIOD_COUNT+1次，为一个定时周期
	DCM_TimeBaseStructure.Init.Period = PWM_PERIOD_COUNT - 1;
	// 通用控制定时器时钟源TIMxCLK = HCLK/2=84MHz 
	// 设定定时器频率为=TIMxCLK/(PWM_PRESCALER_COUNT+1)
  DCM_TimeBaseStructure.Init.Prescaler = PWM_PRESCALER_COUNT - 1;	
	
	/*计数方式*/
  DCM_TimeBaseStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
	/*采样时钟分频*/
  DCM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	/*初始化定时器*/
  HAL_TIM_PWM_Init(&DCM_TimeBaseStructure);
  
	/*PWM模式配置*/
  TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;
	TIM_OCInitStructure.Pulse = 0;
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OCInitStructure.OCNPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_SET;
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;
	
	/*配置PWM通道*/
  HAL_TIM_PWM_ConfigChannel(&DCM_TimeBaseStructure, &TIM_OCInitStructure, PWM_CHANNEL_1);
	/*开始输出PWM*/
	HAL_TIM_PWM_Start(&DCM_TimeBaseStructure,PWM_CHANNEL_1);
	
	/*配置脉宽*/
  TIM_OCInitStructure.Pulse = 0;    // 默认占空比为50%
	/*配置PWM通道*/
  HAL_TIM_PWM_ConfigChannel(&DCM_TimeBaseStructure, &TIM_OCInitStructure, PWM_CHANNEL_2);
	/*开始输出PWM*/
	HAL_TIM_PWM_Start(&DCM_TimeBaseStructure,PWM_CHANNEL_2);
}

/**
  * @brief  设置TIM通道的占空比
	* @param  channel		通道	（1,2,3,4）
	* @param  compare		占空比
	*	@note 	无
  * @retval 无
  */
void TIM1_SetPWM_pulse(uint32_t channel,int compare)
{
		switch(channel)
	{
		case TIM_CHANNEL_1:  	__HAL_TIM_SET_COMPARE(&DCM_TimeBaseStructure,TIM_CHANNEL_1,compare);break;
		case TIM_CHANNEL_2:	  __HAL_TIM_SET_COMPARE(&DCM_TimeBaseStructure,TIM_CHANNEL_2,compare);break;
		case TIM_CHANNEL_3:	  __HAL_TIM_SET_COMPARE(&DCM_TimeBaseStructure,TIM_CHANNEL_3,compare);break;
		case TIM_CHANNEL_4:	  __HAL_TIM_SET_COMPARE(&DCM_TimeBaseStructure,TIM_CHANNEL_4,compare);break;
	}
}


/**
  * @brief  初始化控制通用定时器
  * @param  无
  * @retval 无
  */
void Motor_TIMx_Configuration(void)
{
	TIMx_GPIO_Config();
  
  TIM_PWMOUTPUT_Config();
}




/*********************************************END OF FILE**********************/
