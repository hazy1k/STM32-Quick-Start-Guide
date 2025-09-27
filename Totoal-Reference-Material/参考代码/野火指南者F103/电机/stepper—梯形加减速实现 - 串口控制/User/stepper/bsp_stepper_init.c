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
#include "./stepper/bsp_stepper_init.h"
#include "./delay/core_delay.h"   
#include "stm32f1xx.h"
#include "math.h"
#include <math.h>

TIM_HandleTypeDef TIM_TimeBaseStructure;

/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void Stepper_GPIO_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*开启Motor相关的GPIO外设时钟*/
	MOTOR_DIR_GPIO_CLK_ENABLE();
	MOTOR_PUL_GPIO_CLK_ENABLE();
	MOTOR_EN_GPIO_CLK_ENABLE();

	/*选择要控制的GPIO引脚*/															   
	GPIO_InitStruct.Pin = MOTOR_DIR_PIN;	
	/*设置引脚的输出类型为推挽输出*/
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
	GPIO_InitStruct.Pull =GPIO_PULLUP;
	/*设置引脚速率为高速 */   
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	/*Motor 方向引脚 初始化*/
	HAL_GPIO_Init(MOTOR_DIR_GPIO_PORT, &GPIO_InitStruct);	

	/*Motor 使能引脚 初始化*/
	GPIO_InitStruct.Pin = MOTOR_EN_PIN;	
	HAL_GPIO_Init(MOTOR_EN_GPIO_PORT, &GPIO_InitStruct);	


	/* 定时器通道1功能引脚IO初始化 */
	/*设置输出类型*/
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	/*设置引脚速率 */ 
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	/*设置复用*/
	GPIO_InitStruct.Pull =GPIO_PULLUP;
	/*选择要控制的GPIO引脚*/	
	GPIO_InitStruct.Pin = MOTOR_PUL_PIN;
	/*Motor 脉冲引脚 初始化*/
	HAL_GPIO_Init(MOTOR_PUL_PORT, &GPIO_InitStruct);			
}

 /**
  * @brief  中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIMx_NVIC_Configuration(void)
{
	/* 外设中断配置 */
	HAL_NVIC_SetPriority(MOTOR_PUL_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(MOTOR_PUL_IRQn);
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
void TIM_PWMOUTPUT_Config(void)
{
	TIM_OC_InitTypeDef  TIM_OCInitStructure;  	
	/*使能定时器*/
	MOTOR_PUL_CLK_ENABLE();

	TIM_TimeBaseStructure.Instance = MOTOR_PUL_TIM;    
	/* 累计 TIM_Period个后产生一个更新或者中断*/		
	//当定时器从0计数到10000，即为10000次，为一个定时周期
	TIM_TimeBaseStructure.Init.Period = TIM_PERIOD; 
	// 通用控制定时器时钟源TIMxCLK = HCLK/2	=	84MHz 
	// 高级控制定时器时钟源TIMxCLK = HCLK		=	168MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)
	TIM_TimeBaseStructure.Init.Prescaler = TIM_PRESCALER;                

	/*计数方式*/
	TIM_TimeBaseStructure.Init.CounterMode = TIM_COUNTERMODE_UP;            
	/*采样时钟分频*/	
	TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;   
	TIM_TimeBaseStructure.Init.RepetitionCounter = 0 ;  		
	/*初始化定时器*/
	HAL_TIM_OC_Init(&TIM_TimeBaseStructure);

	/*PWM模式配置--这里配置为输出比较模式*/
	TIM_OCInitStructure.OCMode = TIM_OCMODE_TOGGLE; 
	/*比较输出的计数值*/
	TIM_OCInitStructure.Pulse = 0;                    
	/*当定时器计数值小于CCR1_Val时为高电平*/
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;          
	/*设置互补通道输出的极性*/
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_LOW; 
	/*快速模式设置*/
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;   
	/*空闲电平*/
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_RESET;  
	/*互补通道设置*/
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET; 
	HAL_TIM_OC_ConfigChannel(&TIM_TimeBaseStructure, &TIM_OCInitStructure, MOTOR_PUL_CHANNEL_x);
}


/**
  * @brief  引脚初始化
  * @retval 无
  */
void stepper_Init()
{
	/*电机IO配置*/
	Stepper_GPIO_Config();
	/*定时器PWM输出配置*/
	TIM_PWMOUTPUT_Config();
	/*中断配置*/
	TIMx_NVIC_Configuration();
}

/**
  * @brief  定时器中断服务函数
  * @retval 无
  */
void MOTOR_PUL_IRQHandler(void)
{ 
	/*速度状态决策*/
	speed_decision();
}

