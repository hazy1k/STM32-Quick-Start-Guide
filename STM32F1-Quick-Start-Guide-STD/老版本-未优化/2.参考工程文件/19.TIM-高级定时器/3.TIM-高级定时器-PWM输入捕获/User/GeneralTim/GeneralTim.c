#include "GeneralTim.h"

static void GTIM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// TIM1 CH1 GPIO Configuration
	RCC_APB2PeriphClockCmd(GTIM_CH1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GTIM_CH1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GTIM_CH1_GPIO_PORT, &GPIO_InitStructure);
	// TIM1 CH2 GPIO Configuration
	RCC_APB2PeriphClockCmd(GTIM_CH2_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GTIM_CH2_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GTIM_CH2_GPIO_PORT, &GPIO_InitStructure);
	// TIM1 CH3 GPIO Configuration
	RCC_APB2PeriphClockCmd(GTIM_CH3_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GTIM_CH3_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GTIM_CH3_GPIO_PORT, &GPIO_InitStructure);
	// TIM1 CH4 GPIO Configuration
	RCC_APB2PeriphClockCmd(GTIM_CH4_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GTIM_CH4_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GTIM_CH4_GPIO_PORT, &GPIO_InitStructure);
}

static void GTIM_Mode_Init(void)
{
	GTIM_APBxClock(GTIM_CLK, ENABLE);
	// 时基结构初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = GTIM_Period; // 9
	TIM_TimeBaseStructure.TIM_Prescaler = GTIM_Prescaler; // 71
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(GTIMx, &TIM_TimeBaseStructure);
    // 输出比较模式初始化
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    // CH1
	TIM_OCInitStructure.TIM_Pulse = GTIM_CCR1; // 5
	TIM_OC1Init(GTIMx, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(GTIMx, TIM_OCPreload_Enable); // 预装载使能
	// CH2
	TIM_OCInitStructure.TIM_Pulse = GTIM_CCR2; // 4
	TIM_OC2Init(GTIMx, &TIM_OCInitStructure);
	// CH3
	TIM_OCInitStructure.TIM_Pulse = GTIM_CCR3; // 3
	TIM_OC3Init(GTIMx, &TIM_OCInitStructure);
	// CH4
	TIM_OCInitStructure.TIM_Pulse = GTIM_CCR4; // 2
	TIM_OC4Init(GTIMx, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(GTIMx, TIM_OCPreload_Enable);
	TIM_Cmd(GTIMx, ENABLE);
}

void GTIM_Init(void)
{
	GTIM_GPIO_Init();
	GTIM_Mode_Init();
}
