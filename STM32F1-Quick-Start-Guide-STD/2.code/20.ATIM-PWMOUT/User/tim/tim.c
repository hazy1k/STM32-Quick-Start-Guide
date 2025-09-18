#include "tim.h"
#include "usart.h"
#include <stdio.h>
#include <math.h>

// 计算死区时间 (单位: ns)
static uint32_t calc_deadtime(uint8_t dt)
{
    // 系统时钟频率 (Hz)
    uint32_t system_clk = SystemCoreClock;
    // 定时器时钟频率 (Hz)
    uint32_t tim_clk = system_clk / (TIM1->PSC + 1);
    // 基本时间单位 (ns)
    float t_dts = 1000000000.0f / tim_clk;
    return (uint32_t)(dt * t_dts);
}

static void ATIM_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // PA8: TIM1_CH1 (PWM主输出)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // PB13: TIM1_CH1N (互补输出)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // PB12: 刹车输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  // 输入上拉模式
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void ATIM_Mode_Init(void)
{
    ATIM_GPIO_Init();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    
    /* 时基单元配置 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 7;          // ARR = 7
    TIM_TimeBaseStructure.TIM_Prescaler = 8;        // PSC = 8
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    
    /* 输出比较配置 */
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 4;  // 50%占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    
    /* 刹车和死区配置 */
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
    TIM_BDTRStructInit(&TIM_BDTRInitStructure);
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
    TIM_BDTRInitStructure.TIM_DeadTime = 11;  // 死区时间值
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
    
    // 使能定时器
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    
    // 清除中断标志
    TIM_ClearFlag(TIM1, TIM_FLAG_Update | TIM_FLAG_Break);
}

void TIM1_DebugInfo(void)
{
    // 计算实际输出频率和占空比
    uint32_t system_clk = SystemCoreClock;
    uint32_t tim_clk = system_clk / (TIM1->PSC + 1);
    float output_freq = (float)tim_clk / (TIM1->ARR + 1);
    float duty_cycle = (TIM1->CCR1 * 100.0f) / (TIM1->ARR + 1);
    uint32_t deadtime_ns = calc_deadtime(TIM1->BDTR & 0xFF);
    
    printf("\r\n===== TIM1 Configuration =====\r\n");
    printf("System Clock: %u Hz\r\n", system_clk);
    printf("TIM1 Clock: %u Hz\r\n", tim_clk);
    printf("PSC: %u\r\n", TIM1->PSC);
    printf("ARR: %u\r\n", TIM1->ARR);
    printf("CCR1: %u\r\n", TIM1->CCR1);
    printf("Output Freq: %.1f Hz\r\n", output_freq);
    printf("Duty Cycle: %.1f%%\r\n", duty_cycle);
    printf("Dead Time: %u ns\r\n", deadtime_ns);
    printf("Break Input (PB12): %s\r\n", 
           GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) ? "High" : "Low");
    printf("Break Flag: %s\r\n", 
           TIM_GetFlagStatus(TIM1, TIM_FLAG_Break) ? "Active" : "Inactive");
    printf("MOE Status: %s\r\n", 
           (TIM1->BDTR & TIM_BDTR_MOE) ? "Enabled" : "Disabled");
    printf("TIM1 Status: %s\r\n", 
           (TIM1->CR1 & TIM_CR1_CEN) ? "Enabled" : "Disabled");
    printf("BDTR Register: 0x%04X\r\n", TIM1->BDTR);
    printf("=============================\r\n");
}
