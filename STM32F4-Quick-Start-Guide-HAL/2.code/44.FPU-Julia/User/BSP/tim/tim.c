#include "tim.h"

extern uint8_t timeout;

TIM_HandleTypeDef TIM6_Handler; // TIM6 handler

void BSP_TIM_Init(uint16_t arr, uint16_t psc)
{
    TIM6_Handler.Instance = TIM6;
    TIM6_Handler.Init.Prescaler = psc;
    TIM6_Handler.Init.CounterMode = TIM_COUNTERMODE_UP; // 向上计数模式
    TIM6_Handler.Init.Period = arr;
    TIM6_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 1分频
    HAL_TIM_Base_Init(&TIM6_Handler);
    HAL_TIM_Base_Start_IT(&TIM6_Handler); // 启动定时器中断
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        __HAL_RCC_TIM6_CLK_ENABLE(); // 使能TIM6时钟
        HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 1, 3); 
        HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);        
    }
}

void TIM6_DAC_IRQHandler(void) // 修改为TIM6_DAC_IRQHandler
{
    HAL_TIM_IRQHandler(&TIM6_Handler);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        timeout++;
    }
}
