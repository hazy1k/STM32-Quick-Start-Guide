#include "spwm.h"

__IO uint16_t period_class = 1; // 控制输出波形的频率

// PWM表
const uint16_t indexWave[] = {
0, 9, 18, 27, 36, 45, 54, 63, 72, 81, 89, 98,
	107, 116, 125, 133, 142, 151, 159, 168, 176,
	184, 193, 201, 209, 218, 226, 234, 242, 249,
	257, 265, 273, 280, 288, 295, 302, 310, 317, 
	324, 331, 337, 344, 351, 357, 364, 370, 376, 
	382, 388, 394, 399, 405, 410, 416, 421, 426, 
	431, 436, 440, 445, 449, 454, 458, 462, 465, 
	469, 473, 476, 479, 482, 485, 488, 491, 493, 
	496, 498, 500, 502, 503, 505, 506, 508, 509, 
	510, 510, 511, 512, 512, 512, 512, 512, 512,
	511, 510, 510, 509, 508, 506, 505, 503, 502,
	500, 498, 496, 493, 491, 488, 485, 482, 479,
	476, 473, 469, 465, 462, 458, 454, 449, 445, 
	440, 436, 431, 426, 421, 416, 410, 405, 399, 
	394, 388, 382, 376, 370, 364, 357, 351, 344, 
	337, 331, 324, 	317, 310, 302, 295, 288, 280, 
	273, 265, 257, 249, 242, 234, 226, 218, 209, 
	201, 193, 184, 176, 168, 159, 151, 142, 133, 
125, 116, 107, 98, 89, 81, 72, 63, 54, 45, 36,
27, 18, 9, 0
};

uint16_t PWM_Num = sizeof(indexWave)/sizeof(indexWave[0]); 

static void TIM_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  	NVIC_InitStructure.NVIC_IRQChannel = TIMx_IRQ;
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
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
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
	// 时基结构初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = (512-1);       							 
	TIM_TimeBaseStructure.TIM_Prescaler = (10-1);	    							
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	// PWM模式初始化
  	TIM_OCInitTypeDef TIM_OCInitStructure;																				
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_Pulse = 0;				 						  				
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  
  	RED_TIM_OCxInit(TIMx, &TIM_OCInitStructure );	
	RED_TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable);	
  	GREEN_TIM_OCxInit(TIMx, &TIM_OCInitStructure);	
  	GREEN_TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable);	
  	BLUE_TIM_OCxInit(TIMx, &TIM_OCInitStructure);	
  	BLUE_TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable);							
  	TIM_ARRPreloadConfig(TIMx, ENABLE); // 使能TIM重载寄存器ARR
    TIM_Cmd(TIMx, ENABLE);                   										
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);										
}

void SPWM_Init(void)
{
	TIM_NVIC_Init();
	TIM_GPIO_Init();
	TIM_Mode_Init();
}
