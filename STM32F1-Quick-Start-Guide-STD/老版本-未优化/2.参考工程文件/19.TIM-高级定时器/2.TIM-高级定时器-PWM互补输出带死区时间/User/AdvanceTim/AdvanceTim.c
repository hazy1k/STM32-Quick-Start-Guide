#include "AdvanceTim.h"

static void ATIM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// 输出比较通道GPIO初始化
	RCC_APB2PeriphClockCmd(ATIM_CH1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ATIM_CH1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATIM_CH1_GPIO_PORT, &GPIO_InitStructure);
	// 输出比较通道的互补通道GPIO初始化
	RCC_APB2PeriphClockCmd(ATIM_CH1N_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ATIM_CH1N_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATIM_CH1N_GPIO_PORT, &GPIO_InitStructure);
	// 输出比较通道的刹车通道GPIO初始化
	RCC_APB2PeriphClockCmd(ATIM_BKIN_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ATIM_BKIN_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATIM_BKIN_GPIO_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(ATIM_BKIN_GPIO_PORT, ATIM_BKIN_GPIO_PIN); // BKIN默认是低电平状态  
}

static void ATIM_Mode_Init(void)
{
	ATIM_APBxClock(ATIM_CLK, ENABLE); // 72MHz
	// 时基结构初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = ATIM_Period; // 8-1
	TIM_TimeBaseStructure.TIM_Prescaler = ATIM_Prescaler; // 9-1
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(ATIMx, &TIM_TimeBaseStructure);
	// 输出比较结构初始化
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // 配置为PWM1模式
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 输出使能
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 互补输出使能
	TIM_OCInitStructure.TIM_Pulse = ATIM_Pulse; // 占空比4 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; // 互补通道输出极性高
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set; // 输出通道空闲状态置位
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset; // 互补通道空闲状态复位
	TIM_OC1Init(ATIMx, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(ATIMx, TIM_OCPreload_Enable); // 预装载使能
	// 死区结构初始化
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable; // 允许重装载
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable; // 允许停止
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; // 锁定
	TIM_BDTRInitStructure.TIM_DeadTime = 11; // 死区时间11
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable; // 允许死区
	// 当BKIN引脚检测到高电平的时候，输出比较信号立即被禁止，直到下一个更新事件发生。
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High; // 死区极性高
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable; // 允许自动输出
	TIM_BDTRConfig(ATIMx, &TIM_BDTRInitStructure);
	TIM_Cmd(ATIMx, ENABLE);
	TIM_CtrlPWMOutputs(ATIMx, ENABLE); // 使能PWM输出
}

void ATIMx_Config(void)
{
	ATIM_GPIO_Init();
	ATIM_Mode_Init();
}
