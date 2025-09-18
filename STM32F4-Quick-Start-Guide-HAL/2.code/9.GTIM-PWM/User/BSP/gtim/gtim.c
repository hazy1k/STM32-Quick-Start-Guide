#include <gtim.h>

TIM_HandleTypeDef TIM14_Handler;
TIM_OC_InitTypeDef TIM14_CH1_Handler;

void TIM14_PWM_Init(uint16_t arr, uint16_t psc)
{
    /* Mode Init */
    TIM14_Handler.Instance = TIM14;
    TIM14_Handler.Init.Period = arr;
    TIM14_Handler.Init.Prescaler = psc;
    TIM14_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM14_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM14_Handler);
    /* PWM Init */
    TIM14_CH1_Handler.OCMode = TIM_OCMODE_PWM1; // PWM1模式
    TIM14_CH1_Handler.Pulse = arr/2; // 占空比50%
    TIM14_CH1_Handler.OCPolarity = TIM_OCPOLARITY_LOW;
    HAL_TIM_PWM_ConfigChannel(&TIM14_Handler, &TIM14_CH1_Handler, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TIM14_Handler, TIM_CHANNEL_1);
}

// tim底层驱动，会被HAL_TIM_PWM_Init()调用
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_TIM14_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_TIM14;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

// 设置占空比
void TIM_SetCompare(uint16_t compare)
{
    TIM14->CCR1 = compare;
}
