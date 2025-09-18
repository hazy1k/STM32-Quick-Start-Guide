#include "breathing.h"

__IO uint16_t period_class = 10; // 输出波形的频率

// PWM表
uint16_t indexWave[] = {
1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4,
4, 5, 5, 6, 7, 8, 9, 10, 11, 13,
15, 17, 19, 22, 25, 28, 32, 36,
41, 47, 53, 61, 69, 79, 89, 102,
116, 131, 149, 170, 193, 219, 250,
284, 323, 367, 417, 474, 539, 613,
697, 792, 901, 1024, 1024, 901, 792,
697, 613, 539, 474, 417, 367, 323,
284, 250, 219, 193, 170, 149, 131, 
116, 102, 89, 79, 69, 61, 53, 47, 41,
36, 32, 28, 25, 22, 19, 17, 15, 13, 
11, 10, 9, 8, 7, 6, 5, 5, 4, 4, 3, 3,
2, 2, 2, 2, 1, 1, 1, 1
};

uint16_t PWM_Num = sizeof(indexWave)/sizeof(indexWave[0]); // PWM波形元素个数

static void TIM_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = RED_TIM_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void TIM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RED_TIM_GPIO_CLK, ENABLE);
	RED_TIM_GPIO_APBxClock(RED_TIM_GPIO_CLK, ENABLE);
	RED_GPIO_REMAP_FUN(); // 重新映射GPIO
	// 呼吸灯使用到的引脚配置
	GPIO_InitStructure.GPIO_Pin = RED_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RED_GPIO_PORT, &GPIO_InitStructure);
}

static void TIM_Mode_Init(void)
{
	RED_APBxClock(RED_TIM_CLK, ENABLE);
	// 时基配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = (1024-1);
	TIM_TimeBaseStructure.TIM_Prescaler = (200-1);
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(RED_TIMx, &TIM_TimeBaseStructure);
	// PWM模式配置
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	RED_TIM_OCxInit(RED_TIMx, &TIM_OCInitStructure);
	RED_TIM_OCxPreloadConfig(RED_TIMx, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(RED_TIMx, ENABLE); // 预装载寄存器
	TIM_Cmd(RED_TIMx, ENABLE);
	TIM_ITConfig(RED_TIMx, TIM_IT_Update, ENABLE);
}

void breathing_init(void)
{
	TIM_NVIC_Init();
	TIM_GPIO_Init();
	TIM_Mode_Init();
}

