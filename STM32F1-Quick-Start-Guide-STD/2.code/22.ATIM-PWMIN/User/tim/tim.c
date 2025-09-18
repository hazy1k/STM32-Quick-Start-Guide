#include "tim.h"

// 全局变量存储捕获数据
static volatile PWM_CaptureData pwm_data;
static volatile uint32_t tim1_overflows = 0;

// TIM2 PWM输出初始化 (PA0)
void TIM2_PWM_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    // 1. 使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    // 2. 配置GPIOA.0为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // 3. 配置TIM2时基
    TIM_TimeBaseStructure.TIM_Period = 999;           // 自动重装载值 (1000-1)
    TIM_TimeBaseStructure.TIM_Prescaler = 71;         // 预分频器 (72MHz/72 = 1MHz)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    // 4. 配置PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 300;  // 初始占空比 30%
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    // 5. 使能TIM2
    TIM_Cmd(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
}

// 设置PWM参数
void TIM2_SetPWM(uint32_t period, uint32_t duty_cycle) {
    TIM2->ARR = period - 1;
    TIM2->CCR1 = duty_cycle;
}

// TIM1 PWM输入捕获初始化 (PA8)
void TIM1_PWMCapture_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    // 1. 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA, ENABLE);
    // 2. 配置GPIOA.8为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // 3. 配置TIM1时基
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;        // 最大计数器值
    TIM_TimeBaseStructure.TIM_Prescaler = 0;          // 不分频 (72MHz)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    // 4. 配置PWM输入模式
    // 选择TI1作为输入源
    TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);
    // 从模式: 复位模式 (在上升沿复位计数器)
    TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
    // 使能主从模式
    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
    // 配置通道1 (直接模式): 上升沿捕获
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);
    // 配置通道2 (间接模式): 下降沿捕获
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);
    // 5. 配置中断
    // 更新中断 (用于处理溢出)
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    // 捕获中断 (通道1和通道2)
    TIM_ITConfig(TIM1, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
    // 配置NVIC
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_Init(&NVIC_InitStructure);
    // 6. 使能TIM1
    TIM_Cmd(TIM1, ENABLE);
}

// TIM1更新中断 (处理溢出)
void TIM1_UP_IRQHandler(void) {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
        tim1_overflows++;
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

// TIM1捕获中断
void TIM1_CC_IRQHandler(void) {
    // 通道1捕获 (上升沿 - 周期开始)
    if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET) {
        // 读取周期值 (计数器在上升沿复位，所以CCR1是周期)
        uint32_t capture1 = TIM_GetCapture1(TIM1);
        // 考虑溢出的情况
        if (capture1 == 0) {
            // 如果捕获值为0，则周期是溢出次数 * 0x10000
            pwm_data.period = tim1_overflows * 0x10000;
        } else {
            // 否则周期 = 溢出次数 * 0x10000 + 捕获值
            pwm_data.period = tim1_overflows * 0x10000 + capture1;
        }
        // 重置溢出计数器
        tim1_overflows = 0;
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
    }
    // 通道2捕获 (下降沿 - 占空比)
    if (TIM_GetITStatus(TIM1, TIM_IT_CC2) != RESET) {
        // 读取占空比值 (从上升沿到下降沿的时间)
        pwm_data.duty_cycle = TIM_GetCapture2(TIM1);   
        // 计算频率 (72MHz时钟)
        pwm_data.frequency = 72000000.0f / pwm_data.period;
        // 计算占空比百分比
        pwm_data.duty_ratio = (pwm_data.duty_cycle * 100.0f) / pwm_data.period;
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
    }
}

// 获取PWM捕获数据
PWM_CaptureData TIM1_GetPWMCaptureData(void) {
    return pwm_data;
}
