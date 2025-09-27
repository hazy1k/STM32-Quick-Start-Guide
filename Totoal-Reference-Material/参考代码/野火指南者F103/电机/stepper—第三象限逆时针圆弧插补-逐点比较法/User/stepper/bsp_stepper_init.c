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

TIM_HandleTypeDef TIM_StepperHandle;

Stepper_TypeDef step_motor[2] = 
{
  {X_MOTOR_PUL_PIN, X_MOTOR_DIR_PIN, X_MOTOR_EN_PIN, X_MOTOR_PUL_CHANNEL, X_MOTOR_PUL_PORT, X_MOTOR_DIR_GPIO_PORT, X_MOTOR_EN_GPIO_PORT},
  {Y_MOTOR_PUL_PIN, Y_MOTOR_DIR_PIN, Y_MOTOR_EN_PIN, Y_MOTOR_PUL_CHANNEL, Y_MOTOR_PUL_PORT, Y_MOTOR_DIR_GPIO_PORT, Y_MOTOR_EN_GPIO_PORT},
};

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

/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void Stepper_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 获取数组元素个数 */
  uint8_t member_count = sizeof(step_motor)/sizeof(Stepper_TypeDef);
  
  X_MOTOR_DIR_GPIO_CLK_ENABLE();
  X_MOTOR_EN_GPIO_CLK_ENABLE();
  X_MOTOR_PUL_GPIO_CLK_ENABLE();

  Y_MOTOR_DIR_GPIO_CLK_ENABLE();
  Y_MOTOR_EN_GPIO_CLK_ENABLE();
  Y_MOTOR_PUL_GPIO_CLK_ENABLE();

  for(uint8_t i = 0; i < member_count; i++)
  {
    /*选择要控制的GPIO引脚*/															   
    GPIO_InitStruct.Pin = step_motor[i].dir_pin;	
    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
    GPIO_InitStruct.Pull =GPIO_PULLUP;
    /*设置引脚速率为高速 */   
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    /*Motor 方向引脚 初始化*/
    HAL_GPIO_Init(step_motor[i].dir_port, &GPIO_InitStruct);
    MOTOR_DIR(step_motor[i].dir_port, step_motor[i].dir_pin, CW);
    
    /*Motor 使能引脚 初始化*/
    GPIO_InitStruct.Pin = step_motor[i].en_pin;
    HAL_GPIO_Init(step_motor[i].en_port, &GPIO_InitStruct);
    MOTOR_OFFLINE(step_motor[i].en_port, step_motor[i].en_pin, LOW);
    
    /* 定时器输出通道功能引脚IO初始化 */
    /*设置输出类型*/
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    /*设置引脚速率 */ 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    /*设置复用*/
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    /*选择要控制的GPIO引脚*/	
    GPIO_InitStruct.Pin = step_motor[i].pul_pin;
    /*Motor 脉冲引脚 初始化*/
    HAL_GPIO_Init(step_motor[i].pul_port, &GPIO_InitStruct);
    MOTOR_PUL(step_motor[i].pul_port, step_motor[i].pul_pin, LOW);
  }
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
static void TIM_PWMOUTPUT_Config(void)
{
	TIM_OC_InitTypeDef  TIM_OCInitStructure;

  /* 获取数组元素个数 */
  uint8_t member_count = sizeof(step_motor)/sizeof(Stepper_TypeDef);
  
	/*使能定时器*/
	MOTOR_PUL_CLK_ENABLE();
  
	TIM_StepperHandle.Instance = MOTOR_PUL_TIM;    
	/* 累计 TIM_Period个后产生一个更新或者中断*/		
	//当定时器从0计数到TIM_PERIOD，即为TIM_PERIOD次，为一个定时周期
	TIM_StepperHandle.Init.Period = TIM_PERIOD;
	// 通用控制定时器时钟源TIMxCLK = HCLK = 168MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=28MHz
	TIM_StepperHandle.Init.Prescaler = TIM_PRESCALER-1;

	/*计数方式*/
	TIM_StepperHandle.Init.CounterMode = TIM_COUNTERMODE_UP;            
	/*采样时钟分频*/	
	TIM_StepperHandle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;   
	TIM_StepperHandle.Init.RepetitionCounter = 0;
	/*初始化定时器为输出比较模式*/
  HAL_TIM_Base_Init(&TIM_StepperHandle);

	/*PWM模式配置--这里配置为PWM模式2*/
	TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM2;
	/*比较输出的计数值*/
	TIM_OCInitStructure.Pulse = TIM_PERIOD;
	/*当定时器计数值小于CCR1_Val时为高电平*/
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
	/*设置互补通道输出的极性*/
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH; 
	/*快速模式设置*/
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;
	/*空闲电平*/
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_RESET;  
	/*互补通道设置*/
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  for(uint8_t i = 0; i < member_count; i++)
  {
    /* 配置输出比较通道 */
    HAL_TIM_OC_ConfigChannel(&TIM_StepperHandle, &TIM_OCInitStructure, step_motor[i].pul_channel);
    TIM_CCxChannelCmd(MOTOR_PUL_TIM, step_motor[i].pul_channel, TIM_CCx_DISABLE);
  }
}

/**
  * @brief  步进电机初始化
  * @param  无
	*	@note   无
  * @retval 无
  */
void stepper_Init(void)
{
	/*电机IO配置*/
	Stepper_GPIO_Config();
	/*定时器PWM输出配置*/
	TIM_PWMOUTPUT_Config();
	/*中断配置*/
	TIMx_NVIC_Configuration();
}
