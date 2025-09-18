#include "TIMbase.h"

void TIM_BASE_Init(void)
{
    /* NVIC相关配置 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); // 设置中断优先组
    NVIC_InitStructure.NVIC_IRQChannel = TIM_BASE_IRQ; // 设置中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断
    NVIC_Init(&NVIC_InitStructure); // 初始化NVIC

    /* TIM相关配置 */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = TIM_BASE_Period;	     // ARR = 999
    TIM_TimeBaseStructure.TIM_Prescaler= TIM_BASE_Prescaler; // 预分频值71 
    TIM_TimeBaseInit(TIM_BASE, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM_BASE, TIM_FLAG_Update); // 清除中断标志位
    TIM_ITConfig(TIM_BASE, TIM_IT_Update, ENABLE); // 开启计数器中断
    TIM_Cmd(TIM_BASE, ENABLE);
}
/*  具体计算过程
    定时器时钟源为APB1，APB1为72MHz
    定时器的频率为72/(71+1) = 1MHz
    定时器的计数周期为999+1，计数周期/定时器频率 = 1ms
*/