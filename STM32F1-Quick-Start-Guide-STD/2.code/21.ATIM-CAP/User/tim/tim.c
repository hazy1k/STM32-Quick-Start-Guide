#include "tim.h"

static void TIM_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void TIM_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM_Mode_Init(void)
{
    TIM_GPIO_Init();
    TIM_NVIC_Init();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    /* 时基结构初始化 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = (72 - 1);
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    /* 输入捕获结构初始化 */
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 上升沿捕获
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM5, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM5, TIM_FLAG_Update|TIM_IT_CC1);
    TIM_ITConfig(TIM5, TIM_IT_Update|TIM_IT_CC1, ENABLE);
    TIM_Cmd(TIM5, ENABLE);
}

static volatile uint32_t TIM5_OverflowCount = 0;  // 定时器溢出次数计数器
static volatile uint8_t g_captureState = 0;       // 捕获状态：0-等待上升沿 1-等待下降沿
static volatile uint32_t g_capturePeriod = 0;     // 计算出的脉宽时间（周期）
static volatile uint8_t g_newCaptureAvailable = 0;// 新捕获数据可用标志

void TIM5_IRQHandler(void) {
    // 处理定时器更新中断（溢出）
    if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) {
        TIM5_OverflowCount++;  // 溢出次数增加
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update); // 清除中断标志
    }
    // 处理通道1捕获中断
    if(TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET) {
        if(g_captureState == 0) {  // 第一次捕获：上升沿（按键按下）
            // 1. 清除计数器并重置溢出计数
            TIM5->CNT = 0;
            TIM5_OverflowCount = 0;      
            // 2. 切换为下降沿捕获
            TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Falling);
            // 3. 更新捕获状态
            g_captureState = 1;
        }
        else {  // 第二次捕获：下降沿（按键释放）
            // 1. 获取当前捕获值
            uint32_t captureEnd = TIM_GetCapture1(TIM5);
            // 2. 计算总时间（考虑溢出）
            g_capturePeriod = (TIM5_OverflowCount * 0x10000) + captureEnd;
            // 3. 设置新数据可用标志
            g_newCaptureAvailable = 1;
            // 4. 切换回上升沿捕获（准备下一次测量）
            TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Rising);
            // 5. 重置捕获状态
            g_captureState = 0;
        }
        TIM_ClearITPendingBit(TIM5, TIM_IT_CC1); // 清除捕获中断标志
    }
}

// 获取按键持续时间（微秒）
uint32_t Get_KeyPressDuration(void) {
    g_newCaptureAvailable = 0; // 清除标志
    return g_capturePeriod;
}

// 检查是否有新的捕获数据
uint8_t Is_NewCaptureAvailable(void) {
    return g_newCaptureAvailable;
}
