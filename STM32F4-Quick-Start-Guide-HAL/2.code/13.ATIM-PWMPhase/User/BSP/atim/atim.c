#include <atim.h>

TIM_HandleTypeDef TIM8_Handler;

void TIM8_PWM_Init(uint16_t arr, uint16_t psc)
{
    TIM_OC_InitTypeDef sConfigOC;
    /* TIM8 参数配置 */
    TIM8_Handler.Instance = TIM8;
    TIM8_Handler.Init.Prescaler = psc;
    TIM8_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM8_Handler.Init.Period = arr;
    TIM8_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    TIM8_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_OC_Init(&TIM8_Handler);
    /* PWM CHx配置 */
    sConfigOC.OCMode = TIM_OCMODE_TOGGLE; // 比较输出模式翻转功能
    sConfigOC.Pulse = 250-1; // 输出比较寄存器的值
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH; // 输出比较极性高
    HAL_TIM_OC_ConfigChannel(&TIM8_Handler, &sConfigOC, TIM_CHANNEL_1); // 配置通道1
    __HAL_TIM_ENABLE_OCxPRELOAD(&TIM8_Handler, TIM_CHANNEL_1); // 使能通道1预装载寄存器
    sConfigOC.Pulse = 500-1;
    HAL_TIM_OC_ConfigChannel(&TIM8_Handler, &sConfigOC, TIM_CHANNEL_2); // 配置通道2
    __HAL_TIM_ENABLE_OCxPRELOAD(&TIM8_Handler, TIM_CHANNEL_2);
    sConfigOC.Pulse = 750-1;
    HAL_TIM_OC_ConfigChannel(&TIM8_Handler, &sConfigOC, TIM_CHANNEL_3); // 配置通道3
    __HAL_TIM_ENABLE_OCxPRELOAD(&TIM8_Handler, TIM_CHANNEL_3);
    sConfigOC.Pulse = 1000-1;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET; // 输出比较空闲状态复位
    HAL_TIM_OC_ConfigChannel(&TIM8_Handler, &sConfigOC, TIM_CHANNEL_4); // 配置通道4
    __HAL_TIM_ENABLE_OCxPRELOAD(&TIM8_Handler, TIM_CHANNEL_4);
    HAL_TIM_OC_Start(&TIM8_Handler, TIM_CHANNEL_1); // 开启通道1
    HAL_TIM_OC_Start(&TIM8_Handler, TIM_CHANNEL_2); // 开启通道2
    HAL_TIM_OC_Start(&TIM8_Handler, TIM_CHANNEL_3); // 开启通道3
    HAL_TIM_OC_Start(&TIM8_Handler, TIM_CHANNEL_4); // 开启通道4
}

// GPIO底层驱动
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM8)
    {
        GPIO_InitTypeDef GPIO_Initure;

        __HAL_RCC_TIM8_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();

        GPIO_Initure.Pin = GPIO_PIN_6;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;
        GPIO_Initure.Pull = GPIO_NOPULL;
        GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_Initure.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOC, &GPIO_Initure);
        GPIO_Initure.Pin = GPIO_PIN_7;
        HAL_GPIO_Init(GPIOC, &GPIO_Initure);
        GPIO_Initure.Pin = GPIO_PIN_8;
        HAL_GPIO_Init(GPIOC, &GPIO_Initure);
        GPIO_Initure.Pin = GPIO_PIN_9;
        HAL_GPIO_Init(GPIOC, &GPIO_Initure);
    }
}
