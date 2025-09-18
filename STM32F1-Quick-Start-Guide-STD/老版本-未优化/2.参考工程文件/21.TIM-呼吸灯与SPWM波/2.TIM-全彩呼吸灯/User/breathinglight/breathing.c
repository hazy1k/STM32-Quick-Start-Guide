#include "breathing.h"

// 控制输出波形的频率
__IO uint16_t period_class = 1;
// PWM表
const uint16_t indexWave[] = {
1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 
	2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
	5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10,
	11, 12, 12, 13, 14, 15, 17, 18,
	19, 20, 22, 23, 25, 27, 29, 31, 
	33, 36, 38, 41, 44, 47, 51, 54, 
	58, 63, 67, 72, 77, 83, 89, 95, 
	102, 110, 117, 126, 135, 145, 156, 
	167, 179, 192, 206, 221, 237, 254,
	272, 292, 313, 336, 361, 387, 415,
	445, 477, 512, 512, 477, 445, 415, 
	387, 361, 336, 313, 292, 272, 254, 
	237, 221, 206, 192, 179, 167, 156, 
	145, 135, 126, 117, 110, 102, 95, 
	89, 83, 77, 72, 67, 63, 58, 54, 51, 
	47, 44, 41, 38, 36, 33, 31, 29, 27,
	25, 23, 22, 20, 19, 18, 17, 15, 14,
	13, 12, 12, 11, 10, 9, 9, 8, 8, 7, 7,
	6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 
	3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1
};

uint16_t PWM_Num = sizeof(indexWave)/sizeof(indexWave[0]); // PWM元素个数 

static void TIM_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
	NVIC_InitStructure.NVIC_IRQChannel = TIMx_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
}

static void TIM_GPIO_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(TIM_GPIO_CLK, ENABLE);
	GPIO_REMAP_FUN();
	GPIO_InitStructure.GPIO_Pin = RED_TIM_LED_PIN ;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RED_TIM_LED_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GREEN_TIM_LED_PIN ;
	GPIO_Init(GREEN_TIM_LED_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = BLUE_TIM_LED_PIN ;
	GPIO_Init(BLUE_TIM_LED_PORT, &GPIO_InitStructure);
}

static void TIM_Mode_Init(void)
{
	TIM_APBxClock_FUN(TIM_CLK, ENABLE); 
	/* 时基结构初始化 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = (512-1);       		 
	TIM_TimeBaseStructure.TIM_Prescaler = (10-1);	    		 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	  	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	/* PWM输出通道初始化 */
	TIM_OCInitTypeDef TIM_OCInitStructure;	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    		
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_Pulse = 0;				 			
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	
	/* RGB三个通道输出初始化 */
	RED_TIM_OCxInit(TIMx, &TIM_OCInitStructure );	
	RED_TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable); 
	GREEN_TIM_OCxInit(TIMx, &TIM_OCInitStructure);	
	GREEN_TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable); 
	BLUE_TIM_OCxInit(TIMx, &TIM_OCInitStructure);	
  	BLUE_TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable); 
	TIM_ARRPreloadConfig(TIMx, ENABLE);	
	TIM_Cmd(TIMx, ENABLE); 	
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);		
}

void breathing_Init(void)
{
	TIM_NVIC_Init();
	TIM_GPIO_Init();
	TIM_Mode_Init();	
}
