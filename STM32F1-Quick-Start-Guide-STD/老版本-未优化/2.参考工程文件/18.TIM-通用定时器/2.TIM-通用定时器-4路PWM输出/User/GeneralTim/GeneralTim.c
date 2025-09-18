#include "GeneralTim.h"

// 输出比较通道GPIO配置
static void TIM_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(TIM_CH1_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = TIM_CH1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TIM_CH1_GPIO_PORT, &GPIO_InitStructure); // TIM_CH1 GPIO初始化
    RCC_APB2PeriphClockCmd(TIM_CH2_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = TIM_CH2_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TIM_CH2_GPIO_PORT, &GPIO_InitStructure); // TIM_CH2 GPIO初始化
    RCC_APB2PeriphClockCmd(TIM_CH3_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = TIM_CH3_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TIM_CH3_GPIO_PORT, &GPIO_InitStructure); // TIM_CH3 GPIO初始化
    RCC_APB2PeriphClockCmd(TIM_CH4_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = TIM_CH4_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TIM_CH4_GPIO_PORT, &GPIO_InitStructure); // TIM_CH4 GPIO初始化
}

// TIM Mode配置
static void TIM_Mode_Init(void)
{
    TIM_APBxClock(TIMx_CLK, ENABLE);
/*-------------------------时基配置-------------------------*/
    // 计数器时钟 = 系统时钟 / (分频系数 + 1) = 72M / (71 + 1) = 1MHz
    // PWM信号周期 = (ARR+1)*(PSC+1)/CNTxCLK = (ARR+1)*1
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = TIMx_Period; // 时钟周期9
    TIM_TimeBaseStructure.TIM_Prescaler = TIMx_Prescaler; // 分频系数71
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频系数1
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure); // TIMx时基初始化
/*----------------------输出比较通道配置----------------------*/
    uint16_t CCR1_Val = 5; // 通道1的占空比50%
    uint16_t CCR2_Val = 4; // 通道2的占空比40%
    uint16_t CCR3_Val = 3; // 通道3的占空比30%
    uint16_t CCR4_Val = 2; // 通道4的占空比20%
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 极性高
    // 配置TIM3_CH1
    TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
    TIM_OC1Init(TIMx, &TIM_OCInitStructure);
    TIM_OC1PolarityConfig(TIMx, TIM_OCPreload_Enable); // 使能预装载
    // 配置TIM3_CH2
    TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
    TIM_OC2Init(TIMx, &TIM_OCInitStructure);
    TIM_OC2PolarityConfig(TIMx, TIM_OCPreload_Enable);
    // 配置TIM3_CH3
    TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
    TIM_OC3Init(TIMx, &TIM_OCInitStructure);
    TIM_OC3PolarityConfig(TIMx, TIM_OCPreload_Enable);
    // 配置TIM3_CH4
    TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
    TIM_OC4Init(TIMx, &TIM_OCInitStructure);
    TIM_OC4PolarityConfig(TIMx, TIM_OCPreload_Enable);
    TIM_Cmd(TIMx, ENABLE);
}

void GeneralTim_Init(void)
{
    TIM_GPIO_Init();
    TIM_Mode_Init();
}

