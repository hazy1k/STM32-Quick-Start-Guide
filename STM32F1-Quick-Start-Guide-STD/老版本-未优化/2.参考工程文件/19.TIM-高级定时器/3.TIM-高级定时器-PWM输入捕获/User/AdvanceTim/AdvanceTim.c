#include "AdvanceTim.h"

static void ATIM_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = ATIM_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void ATIM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(ATIM_CH1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ATIM_CH1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(ATIM_CH1_GPIO_PORT, &GPIO_InitStructure);
}

static void ATIM_Mode_Init(void)
{
	ATIM_APBxClock(ATIM_CLK, ENABLE); // 72 MHz
	// 时基结构初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = ATIM_Period; // 999
	TIM_TimeBaseStructure.TIM_Prescaler = ATIM_Prescaler; // 71
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ATIMx, &TIM_TimeBaseStructure);
	// 输入捕获结构初始化
	// 捕获PWM时需要两个捕获寄存器，一个测周期，一个测占空比
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = ATIM_IC1PWM_CHANNEL;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 捕获上升沿
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0; // 不滤波
	TIM_PWMIConfig(ATIMx, &TIM_ICInitStructure);
	/* 当工作做PWM输入模式时，只需要设置触发信号的那一路即可（用于测量周期
	   另外一路（用来测量占空比）会由硬件自带设置，所以不需要设置。*/
	// 捕获通道IC2配置
	/*TIM_ICInitStructure.TIM_Channel = ATIM_IC1PWM_CHANNEL;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; // 捕获下降沿
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0; 
	TIM_PWMIConfig(ATIMx, &TIM_ICInitStructure);*/
	// 选择输入捕获的触发信号
	TIM_SelectInputTrigger(ATIMx, TIM_TS_TI1FP1); // 选择TI1FP1引脚作为触发信号
	// 选择从模式：复位模式
	// PWM输入模式时，从模式工作在复位模式，当捕获开始时，计数器CNT复位
	TIM_SelectSlaveMode(ATIMx, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(ATIMx, TIM_MasterSlaveMode_Enable); // 使能主从模式
	TIM_ITConfig(ATIMx, TIM_IT_CC1, ENABLE);
	TIM_ClearITPendingBit(ATIMx, TIM_IT_CC1);
	TIM_Cmd(ATIMx, ENABLE);
}

void ATIMx_Init(void)
{
	ATIM_NVIC_Init();
	ATIM_GPIO_Init();
	ATIM_Mode_Init();
}
