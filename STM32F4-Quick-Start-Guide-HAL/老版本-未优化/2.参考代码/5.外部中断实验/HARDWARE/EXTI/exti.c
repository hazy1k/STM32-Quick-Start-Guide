#include "exti.h"
#include "key.h"
#include "delay.h"
#include "led.h"

// 中断回调函数
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY_UP_PIN); // 调用HAL库的中断处理函数
}
void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY2_PIN);
}
void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY1_PIN);
}
void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY0_PIN);
}

// 中断服务函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(100);
    switch(GPIO_Pin)
    {
        case KEY_UP_PIN:
            if(KEY_UP_GET == 1) // KEY_UP按下
            {
                LED1 = !LED1;
                LED0 = !LED1;
            }
            break;
        case KEY0_PIN:
            if(KEY0_GET == 0)
            {
                LED0 = !LED0;
            }
            break;
        case KEY2_PIN:
            if(KEY2_GET == 0)
            {
                LED1 = !LED1;
            }
            break;
        case KEY1_PIN:
            if(KEY1_GET == 0)
            {
                LED0 = !LED0;
                LED1 = !LED1;
            }
            break;
    }
}

void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    // KEY_UP高电平有效
    GPIO_InitStructure.Pin = KEY_UP_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING; // 上升沿触发
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    // 普通按键低电平有效
    GPIO_InitStructure.Pin = KEY0_PIN | KEY1_PIN | KEY2_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING; // 下降沿触发
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
    // NVIC配置
    // 中断线0即PA0连接到EXTI0（KEY_up）
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0); // 抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI0_IRQn); // 使能中断
    // 中断线2即PE2连接到EXTI2（KEY2）
    HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 1); // 抢占优先级为2，子优先级为
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
    // 中断线3即PE3连接到EXTI3（KEY1）
    HAL_NVIC_SetPriority(EXTI3_IRQn, 2, 2); // 抢占优先级为2，子优先级为
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    // 中断线4即PE4连接到EXTI4（KEY0）
    HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 3); // 抢占优先级为2，子优先级为
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

