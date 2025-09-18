#include <gtim.h>
#include <usart.h>
#include <stdio.h>

TIM_HandleTypeDef TIM3_Handler;

void TIMx_Init(uint16_t arr, uint16_t psc)
{
    TIM3_Handler.Instance = TIM3;
    TIM3_Handler.Init.Period = arr;
    TIM3_Handler.Init.Prescaler = psc;
    TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM3_Handler);
    HAL_TIM_Base_Start_IT(&TIM3_Handler);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
        HAL_NVIC_SetPriority(GTIMx_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(GTIMx_IRQn);
    }
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler); 
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
        printf("TIM3_IRQHandler!!!\r\n");
        LED_TOGGLE(LED1_GPIO_Pin);
    }
}
