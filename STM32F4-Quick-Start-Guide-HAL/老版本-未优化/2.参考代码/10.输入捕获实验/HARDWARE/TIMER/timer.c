#include "timer.h"
#include "led.h"

TIM_HandleTypeDef TIM14_Handle; // 定时器14句柄
TIM_OC_InitTypeDef TIM14_CH1Handle; // 定时器14通道1句柄
TIM_HandleTypeDef TIM5_Handle; // 定时器5句柄

// TIM14 PWM初始化
void TIM14_PWM_Init(uint16_t arr, uint16_t psc)
{
    TIM14_Handle.Instance = TIM14;
    TIM14_Handle.Init.Prescaler = psc;
    TIM14_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM14_Handle.Init.Period = arr;
    TIM14_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM14_Handle);
    TIM14_CH1Handle.OCMode = TIM_OCMODE_PWM1;
    TIM14_CH1Handle.Pulse = arr/2;
    TIM14_CH1Handle.OCPolarity = TIM_OCNPOLARITY_LOW;
    HAL_TIM_PWM_ConfigChannel(&TIM14_Handle, &TIM14_CH1Handle, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TIM14_Handle, TIM_CHANNEL_1);
}

// 定时器14底层驱动
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_TIM14_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_TIM14; // PF9复用为TIM14_CH1
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

// 设置TIM14通道的占空比
void TIM_SetTIM14Compare1(uint16_t compare)
{
    TIM14->CCR1 = compare;
}

// 定时器5的输入捕获初始化
void TIM5_CH1_Cap_Init(uint32_t arr, uint16_t psc)
{
    TIM_IC_InitTypeDef TIM_ICInitStructure;
    TIM5_Handle.Instance = TIM5;
    TIM5_Handle.Init.Prescaler = psc;
    TIM5_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM5_Handle.Init.Period = arr;
    TIM5_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM5_Handle);
    // 上升沿捕获
    TIM_ICInitStructure.ICPolarity = TIM_ICPOLARITY_RISING;
    TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI; // 映射到输入捕获通道
    TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1; // 不分频
    TIM_ICInitStructure.ICFilter = 0; // 不滤波
    HAL_TIM_IC_ConfigChannel(&TIM5_Handle, &TIM_ICInitStructure, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&TIM5_Handle, TIM_CHANNEL_1); // 开启输入捕获中断
    __HAL_TIM_ENABLE_IT(&TIM5_Handle, TIM_IT_UPDATE); // 开启更新中断
}

// 定时器5底层驱动
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_TIM5_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM5; // PA0复用为TIM5_CH1
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(TIM5_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM5_IRQn);
}

// 定时器5中断服务函数
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM5_Handle);
}

// 设置TIM捕获/比较寄存器值
uint32_t TIM_GetTIM14Capture1(void)
{
    return HAL_TIM_ReadCapturedValue(&TIM14_Handle, TIM_CHANNEL_1); // 获取捕获值
}

// 捕获状态
// [7]:0,没有成功捕获；1,成功捕获
// [6]:0,没有捕获到低电平；1,捕获到低电平
// [5:0]:捕获低电平后溢出的次数（对于32位定时器，1us计数器加1，则溢出次数为2^32-1 = 4294秒）
uint8_t TIM5CH1_CapTure_STA = 0; // 输入捕获标志
uint32_t TIM5CH1_CapTure_VAL; // 输入捕获值

// 定时器更新中断（计数溢出）中断处理回调函数
// 此函会被HAL_TIM_IRQHandler调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) // 计时器溢出中断
{
    if((TIM5CH1_CapTure_STA & 0x80) == 0) // 还未成功捕获
    {
        if(TIM5CH1_CapTure_STA & 0x40) // 已经捕获到高电平
        {
            if((TIM5CH1_CapTure_STA & 0x3F) == 0x3F) // 高电平太长了
            {
                TIM5CH1_CapTure_STA |= 0x80; // 成功捕获标志
                TIM5CH1_CapTure_VAL = 0xFFFFFFFF; // 捕获值设为最大 
            }
        }
        else // 捕获到一次低电平
        {
            TIM5CH1_CapTure_STA++;
        }

    }
}

// 定时器输入捕获中断处理回调函数
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) // 输入捕获中断
{
    if((TIM5CH1_CapTure_STA & 0x80) == 0) // 还未成功捕获
    {
        if(TIM5CH1_CapTure_STA & 0x40) // 捕捉到一个下降沿
        {
            TIM5CH1_CapTure_STA |= 0x80; // 成功捕获标志
            TIM5CH1_CapTure_VAL = HAL_TIM_ReadCapturedValue(&TIM5_Handle, TIM_CHANNEL_1); // 获取当前捕获值
            TIM_RESET_CAPTUREPOLARITY(&TIM5_Handle, TIM_CHANNEL_1); // 复位捕获极性
            TIM_SET_CAPTUREPOLARITY(&TIM5_Handle, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING); // 切换到捕捉上升沿
        }
        else // 捕捉到一个上升沿
        {
            TIM5CH1_CapTure_STA = 0; // 复位状态
            TIM5CH1_CapTure_VAL = 0; // 捕获值清零
            TIM5CH1_CapTure_STA |= 0x40; // 捕获到上升沿标志
            __HAL_TIM_DISABLE(&TIM5_Handle); // 关闭定时器
            __HAL_TIM_SET_COUNTER(&TIM5_Handle, 0); // 复位计数器
            TIM_RESET_CAPTUREPOLARITY(&TIM5_Handle, TIM_CHANNEL_1); // 复位捕获极性
            TIM_SET_CAPTUREPOLARITY(&TIM5_Handle, TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING); // 切换到捕捉下降沿
            __HAL_TIM_ENABLE(&TIM5_Handle); // 开启定时器
        }
    }
}
