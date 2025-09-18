#include "dac.h"

// 定时器PWM模拟DAC
TIM_HandleTypeDef tim_handle;
TIM_OC_InitTypeDef tim_oc_init;

void pwmdac_init(uint16_t arr, uint16_t psc)
{
    tim_handle.Instance = TIM9;
    tim_handle.Init.Prescaler = psc;
    tim_handle.Init.Period = arr;
    tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_PWM_Init(&tim_handle);

    tim_oc_init.OCMode = TIM_OCMODE_PWM1;
    tim_oc_init.Pulse = arr/2;
    tim_oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_PWM_ConfigChannel(&tim_handle, &tim_oc_init, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&tim_handle, TIM_CHANNEL_2);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef gpio_init;
    if(htim->Instance == TIM9)
    {
        __HAL_RCC_TIM9_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        gpio_init.Pin = GPIO_PIN_3;
        gpio_init.Mode = GPIO_MODE_AF_PP;
        gpio_init.Pull = GPIO_PULLUP;
        gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init.Alternate = GPIO_AF3_TIM9;
        HAL_GPIO_Init(GPIOA, &gpio_init);
    }
}

void pwmdac_set_value(uint16_t value)
{
    float temp = value;
    temp /= 100;
    temp = temp*256/3.3f;
    __HAL_TIM_SET_COMPARE(&tim_handle, TIM_CHANNEL_4, temp);
}
