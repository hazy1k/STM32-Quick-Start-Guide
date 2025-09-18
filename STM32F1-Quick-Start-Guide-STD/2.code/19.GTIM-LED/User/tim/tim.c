#include "tim.h"
#include "led.h"

volatile uint32_t time = 0;

static void TIM_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM_Mode_Init(void)
{
    TIM_NVIC_Init();
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); // 计数器溢出中断使能
    TIM_Cmd(TIM6, ENABLE);
}
/*  
    具体计算过程
    定时器时钟源为APB1，APB1为72MHz
    定时器的频率为72/(71+1) = 1MHz
    定时器的计数周期为999+1，计数周期/定时器频率 = 1ms
*/

void TIM6_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        time++;
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}
