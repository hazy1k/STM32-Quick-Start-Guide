#include "beep.h"

// 蜂鸣器GPIO初始化函数
void BEEP_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // 定义GPIO初始化结构体变量
    RCC_APB2PeriphClockCmd(BEEP_GPIO_CLK, ENABLE); // 使能GPIO时钟
    GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN; // 指定GPIO引脚
    GPIO_InitStructure.GPIO_Mode = BEEP_GPIO_Mode; // 指定GPIO模式
    GPIO_InitStructure.GPIO_Speed = BEEP_GPIO_Speed; // 指定GPIO速度
    GPIO_Init(BEEP_GPIO, &GPIO_InitStructure); // 初始化GPIO
}
