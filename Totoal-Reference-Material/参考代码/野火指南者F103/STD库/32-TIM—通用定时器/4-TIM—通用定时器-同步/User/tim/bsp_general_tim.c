/**
  ******************************************************************************
  * @file    bsp_general_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   通用定时器级联同步PWM输出范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 指南者 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_general_tim.h"

/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIMx_GPIO_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

  /* 使能GPIO时钟 */
  RCC_APB2PeriphClockCmd(GENERAL_MASTER_GPIO_CLK |GENERAL_SLAVE1_GPIO_CLK | GENERAL_SLAVE11_GPIO_CLK, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能AFIO时钟
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);//完全重映像TIM2_CH3->PB10
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);//完全重映像TIM3_CH1->PC6

  /* GPIO 配置: PC6(TIM3 CH1) 作为推挽复用输出功能 ------*/
  GPIO_InitStructure.GPIO_Pin = GENERAL_SLAVE1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
  GPIO_Init(GENERAL_SLAVE1_GPIO_PORT, &GPIO_InitStructure);

  /* GPIO 配置: PB6(TIM4 CH1) 作为推挽复用输出功能 ------*/
  GPIO_InitStructure.GPIO_Pin = GENERAL_SLAVE11_PIN;
  GPIO_Init(GENERAL_SLAVE11_GPIO_PORT, &GPIO_InitStructure);

  /* GPIO 配置: PB10(TIM2 CH3) 作为推挽复用输出功能 ------*/
  GPIO_InitStructure.GPIO_Pin = GENERAL_MASTER_PIN;
  GPIO_Init(GENERAL_MASTER_GPIO_PORT, &GPIO_InitStructure);

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
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	  /* 使能TIM时钟  */
  RCC_APB1PeriphClockCmd(GENERAL_TIM_MASTER_CLK | GENERAL_TIM_SLAVE1_CLK |
                         GENERAL_TIM_SLAVE11_CLK, ENABLE);
  /* Timers synchronisation in cascade mode ------------------------------------
     1/TIM2 is configured as Master Timer:
         - PWM Mode is used
         - The TIM2 Update event is used as Trigger Output  

     2/TIM3 is slave for TIM2 and Master for TIM4,
         - PWM Mode is used
         - The ITR1(TIM2) is used as input trigger 
         - Gated mode is used, so start and stop of slave counter
           are controlled by the Master trigger output signal(TIM2 update event).
         - The TIM3 Update event is used as Trigger Output. 

     3/TIM4 is slave for TIM3,
         - PWM Mode is used
         - The ITR2(TIM3) is used as input trigger
         - Gated mode is used, so start and stop of slave counter
           are controlled by the Master trigger output signal(TIM3 update event).

     In this example TIM2 input clock (TIM2CLK) is set to 2 * APB1 clock (PCLK1), 
     since APB1 prescaler is different from 1.   
     TIM2CLK = 2 * PCLK1  
     PCLK1 = HCLK / 4 
     => TIM2CLK = HCLK / 2 = SystemCoreClock /2

     The Master Timer TIM2 is running at TIM2 counter clock:
     TIM2 frequency = (TIM2 counter clock)/ (TIM2 period + 1) = 500 KHz 
     and the duty cycle = TIM2_CCR1/(TIM2_ARR + 1) = 25%.

     The TIM3 is running:
     - At (TIM2 frequency)/ (TIM3 period + 1) = 100 KHz and a duty cycle
       equal to TIM3_CCR1/(TIM3_ARR + 1) = 20%

     The TIM4 is running:
     - At (TIM3 frequency)/ (TIM4 period + 1) = 20 KHz and a duty cycle
       equal to TIM4_CCR1/(TIM4_ARR + 1) = 20%
  
     Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f10x.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect. 
  --------------------------------------------------------------------------- */

  /* 时基配置 */
  TIM_TimeBaseStructure.TIM_Period = 72;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(GENERAL_TIM_MASTER, &TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Period = 4;
  TIM_TimeBaseInit(GENERAL_TIM_SLAVE1, &TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Period = 4;
  TIM_TimeBaseInit(GENERAL_TIM_SLAVE11, &TIM_TimeBaseStructure);

  /* 主 设置成PWM1模式 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 36;//占空比
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(GENERAL_TIM_MASTER, &TIM_OCInitStructure);//初始化 TIM2 输出通道3
  /* 使能“主从模式” */
  TIM_SelectMasterSlaveMode(GENERAL_TIM_MASTER, TIM_MasterSlaveMode_Enable);
	/* 输出触发源选项 */
  TIM_SelectOutputTrigger(GENERAL_TIM_MASTER, TIM_TRGOSource_Update);

	/* 从 设置成PWM1模式 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1;
  TIM_OC1Init(GENERAL_TIM_SLAVE1, &TIM_OCInitStructure);//初始化 TIM3 输出通道1
	TIM_OC1Init(GENERAL_TIM_SLAVE11, &TIM_OCInitStructure);//初始化 TIM4 输出通道1
  /* 使能“主从模式” */
  TIM_SelectMasterSlaveMode(GENERAL_TIM_SLAVE1, TIM_MasterSlaveMode_Enable);
  /* 输出触发源选项 */
  TIM_SelectOutputTrigger(GENERAL_TIM_SLAVE1, TIM_TRGOSource_Update);
	
  TIM_SelectSlaveMode(GENERAL_TIM_SLAVE1, TIM_SlaveMode_Gated);//从模式配置 : TIM3
  TIM_SelectInputTrigger(GENERAL_TIM_SLAVE1, TIM_TS_ITR1);//设置输入触发源
	
  TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Gated);//从模式配置 : TIM4
  TIM_SelectInputTrigger(GENERAL_TIM_SLAVE11, TIM_TS_ITR2);//设置输入触发源
  
  /* TIM 计数器溢出中断启用 */
  TIM_Cmd(GENERAL_TIM_SLAVE1,ENABLE);
  TIM_Cmd(GENERAL_TIM_MASTER,ENABLE);
  TIM_Cmd(GENERAL_TIM_SLAVE11,ENABLE);
}

/**
  * @brief  初始化通用定时器级联同步PWM输出
  * @param  无
  * @retval 无
  */
void TIMx_Configuration(void)
{  
	TIMx_GPIO_Config();
	
  TIM_Mode_Config();
}

/*********************************************END OF FILE**********************/
