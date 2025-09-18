#include "key.h"

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 配置中断优先级组
    NVIC_InitStructure.NVIC_IRQChannel = KEY1_EXTI_IRQ; // 配置中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 配置中断优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 配置中断子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = KEY2_EXTI_IRQ;
    NVIC_Init(&NVIC_InitStructure);
}

void KEY_EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    RCC_APB2PeriphClockCmd(KEY1_EXTI_CLK|KEY2_EXTI_CLK, ENABLE);
    NVIC_Configuration();
    /* GPIO 配置 */
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_Pin;
    GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStructure);
    /* EXTI 配置 */
    GPIO_EXTILineConfig(KEY1_EXTI_Port, KEY1_EXTI_Pin); // 配置中断引脚
    EXTI_InitStructure.EXTI_Line = KEY1_EXTI_Line; // 配置中断线
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; // 配置中断模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE; // 使能中断线
    EXTI_Init(&EXTI_InitStructure);
    GPIO_EXTILineConfig(KEY2_EXTI_Port, KEY2_EXTI_Pin);
    EXTI_InitStructure.EXTI_Line = KEY2_EXTI_Line;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}
