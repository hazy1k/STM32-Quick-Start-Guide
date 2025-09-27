/**
  ******************************************************************************
  * @file    bsp_pwm_output.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   tim3输出四路pwm bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "..\User\bsp\breathinglight\bsp_breathing.h" 

 /**
  * @brief  配置TIM3复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIM3_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE); 

  	/*IO设置*/
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
  
  /* 配置呼吸灯用到的PB0引脚 */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_5 ;
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);

}

/**
  * @brief  配置TIM3输出的PWM信号的模式，如周期、极性
  * @param  无
  * @retval 无
  */

/*
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> 中断 & TIMxCNT 重新计数
 *                    TIMx_CCR(电平发生变化)
 *
 * 信号周期=(TIMx_ARR +1 ) * 时钟周期
 * 
 */

/*    _______    ______     _____      ____       ___        __         _
 * |_|       |__|      |___|     |____|    |_____|   |______|  |_______| |________|
 */
static void TIM3_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;																				
	
	/* 设置TIM3CLK 时钟为72MHZ */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 					//使能TIM3时钟

  /* 基本定时器配置 */		 
  TIM_TimeBaseStructure.TIM_Period = 255;       							  //当定时器从0计数到255，即为266次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = 1999;	    							//设置预分频：
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//设置时钟分频系数：不分频(这里用不到)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//向上计数模式
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWM模式配置 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
  TIM_OCInitStructure.TIM_Pulse = 0;										  			//设置初始PWM脉冲宽度为0	
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  //当定时器计数值小于CCR1_Val时为低电平

  TIM_OC2Init(TIM3, &TIM_OCInitStructure);	 									//使能通道3
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);						//使能预装载	
  
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);	 									//使能通道3
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);						//使能预装载	
  
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);	 									//使能通道3
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);						//使能预装载	

  TIM_ARRPreloadConfig(TIM3, ENABLE);			 										//使能TIM3重载寄存器ARR

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);                   										//使能定时器3		
}

/**
  * @brief  TIM3 呼吸灯初始化
  *         配置PWM模式和GPIO
  * @param  无
  * @retval 无
  */
void TIM3_Breathing_Init(void)
{
	TIM3_GPIO_Config();
	TIM3_Mode_Config();	
}

//停止pwm输出
void TIM3_Breathing_Close(void)
{
	SetColorValue(0,0,0);
	TIM_ForcedOC2Config(TIM3,TIM_ForcedAction_InActive);
	TIM_ForcedOC3Config(TIM3,TIM_ForcedAction_InActive);
	TIM_ForcedOC4Config(TIM3,TIM_ForcedAction_InActive);
	TIM_ARRPreloadConfig(TIM3, DISABLE);
	TIM_Cmd(TIM3, DISABLE);                   							//失能定时器3						
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE); 	//失能定时器3时钟
	GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_5);
}
//RGBLED显示颜色
void SetRGBColor(uint32_t rgb)
{
	uint8_t r=0,g=0,b=0;
	r=(uint8_t)(rgb>>16);
	g=(uint8_t)(rgb>>8);
	b=(uint8_t)rgb;
	TIM3->CCR2 = r;	//根据PWM表修改定时器的比较寄存器值
	TIM3->CCR3 = g;	//根据PWM表修改定时器的比较寄存器值        
	TIM3->CCR4 = b;	//根据PWM表修改定时器的比较寄存器值
}
//RGBLED显示颜色
void SetColorValue(uint8_t r,uint8_t g,uint8_t b)
{
	TIM3->CCR2 = r;	//根据PWM表修改定时器的比较寄存器值
	TIM3->CCR3 = g;	//根据PWM表修改定时器的比较寄存器值        
	TIM3->CCR4 = b;	//根据PWM表修改定时器的比较寄存器值
}
/*********************************************END OF FILE**********************/
