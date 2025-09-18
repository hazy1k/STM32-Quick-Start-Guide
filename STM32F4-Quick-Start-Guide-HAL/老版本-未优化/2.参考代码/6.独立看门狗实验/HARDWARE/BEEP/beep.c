#include "beep.h"

void BEEP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOF_CLK_ENABLE(); // 使能GPIOF时钟
    GPIO_InitStructure.Pin = BEEP_PIN; // PF8
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; // 推挽输出
    GPIO_InitStructure.Pull = GPIO_PULLUP; // 上拉
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH; // 高速
    HAL_GPIO_Init(GPIOF, &GPIO_InitStructure); // 初始化GPIO
    BEEP_OFF(); // 默认熄灭蜂鸣器
}
