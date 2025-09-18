#include "GeneralTim.h"

static void TIM_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = TIM_IC_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void TIM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(TIM_CH1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TIM_CH1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 输入浮空模式
	GPIO_Init(TIM_CH1_GPIO_PORT, &GPIO_InitStructure);
}

static void TIM_Mode_Config(void)
{
	TIM_APBxClock(TIMx_CLK, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = TIMx_Period; // 0xFFFF
	TIM_TimeBaseStructure.TIM_Prescaler = TIMx_Prescaler; // 71
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	// 输入捕获配置
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_CHx; // TIM5_CH1
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 上升沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0; // 不滤波
	TIM_ICInit(TIMx, &TIM_ICInitStructure);
	
	TIM_ClearFlag(TIMx, TIM_FLAG_Update|TIM_IT_CCx); // 清除中断标志位
	TIM_ITConfig(TIMx, TIM_IT_Update|TIM_IT_CCx, ENABLE); // 使能更新中断和通道捕获中断
	TIM_Cmd(TIMx, ENABLE);
}

void GTim_Init(void)
{
	TIM_NVIC_Init();
	TIM_GPIO_Init();
	TIM_Mode_Config();
}
