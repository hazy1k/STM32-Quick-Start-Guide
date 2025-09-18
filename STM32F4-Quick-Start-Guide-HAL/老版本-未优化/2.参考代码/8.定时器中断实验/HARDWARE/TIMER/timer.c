#include "timer.h"
#include "led.h"

TIM_HandleTypeDef TIM3_Handler; // 声明一个定时器句柄
// 前置知识
/*
    arr:自动重装载值，计数器到达该值时，计数器自动重装载，计数器重新计数。
    psc:预分频值，用来设置时钟源的频率，以此来控制计数器的频率。
    定时器溢出时间计算公式：
    Time= ((arr+1)*(psc+1))/fclk us ,这里的fclk是定时器工作频率，单位为Hz。
    我们这个工程使用的是定时器3，定时器挂在APB1上，时钟位HCLK/2
*/
void TIM_Config(uint16_t arr, uint16_t psc)
{
    TIM3_Handler.Instance = TIM3;
    TIM3_Handler.Init.Prescaler = psc;
    TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP; // 向上计数模式
    TIM3_Handler.Init.Period = arr;
    TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 时钟分频系数为1
    HAL_TIM_Base_Init(&TIM3_Handler);
    HAL_TIM_Base_Start_IT(&TIM3_Handler); // 开启定时器中断
}

// 此函数会被HAL_TIM_Base_Init()调用，用来初始化定时器
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM_IRQn, 1, 3);
        HAL_NVIC_EnableIRQ(TIM_IRQn); // 开启定时器中断
    }
}

// 中断服务函数
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler); // 调用HAL库的中断处理函数
}

// 回调函数，中断服务函数会调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        LED1 = !LED1;
    }
}
