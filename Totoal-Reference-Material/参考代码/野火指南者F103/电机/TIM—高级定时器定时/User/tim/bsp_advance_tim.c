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

TIM_HandleTypeDef TIM_TimeBaseStructure;
 /**
  * @brief  高级控制定时器 TIMx,x[1,8]中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIMx_NVIC_Configuration(void)
{	
	//设置抢占优先级，子优先级
	HAL_NVIC_SetPriority(ADVANCE_TIM_IRQn, 0, 3);
	// 设置中断来源
	HAL_NVIC_EnableIRQ(ADVANCE_TIM_IRQn);
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

	// 开启TIMx_CLK
	ADVANCE_TIM_CLK_ENABLE(); 
	
	TIM_TimeBaseStructure.Instance = ADVANCE_TIM;
	/* 累计 TIM_Period个后产生一个更新或者中断*/		
	//当定时器从0计数到4999，即为5000次，为一个定时周期
	TIM_TimeBaseStructure.Init.Period = 5000-1;       
	//定时器时钟源TIMxCLK = PCLK2 = HCLK
	//=> TIMxCLK=HCLK=SystemCoreClock=72MHz
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=10KHz
	TIM_TimeBaseStructure.Init.Prescaler = 7200-1;	
	// 采样时钟分频
	TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	// 计数方式
	TIM_TimeBaseStructure.Init.CounterMode=TIM_COUNTERMODE_UP;
	// 重复计数器:重复1次，即计数两次才生成一个中断
	TIM_TimeBaseStructure.Init.RepetitionCounter=1;

	// 初始化定时器TIMx, x[1,8]
	HAL_TIM_Base_Init(&TIM_TimeBaseStructure);

	// 开启定时器更新中断
	HAL_TIM_Base_Start_IT(&TIM_TimeBaseStructure);	
}

/**
  * @brief  初始化高级控制定时器定时，1s产生一次中断
  * @param  无
  * @retval 无
  */
void TIMx_Configuration(void)
{
	TIMx_NVIC_Configuration();	

	TIM_Mode_Config();
}

/*********************************************END OF FILE**********************/
