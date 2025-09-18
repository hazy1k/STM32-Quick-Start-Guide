#include "iwdg.h"

IWDG_HandleTypeDef IWDG_Handler;

// 前置知识
/*
    IWDG配置需要两个参数：
    分频系数：IWDG_Prescaler_4 ~ IWDG_Prescaler_256
    自动重装载值：0~0x0FFF
    时间计算公式：
    Time = ((分频系数*重装载值)/LSI频率
    其中LSI频率通常为32.768KHz
*/

void IWDG_Init(uint8_t prescaler, uint16_t reload)
{
    IWDG_Handler.Instance = IWDG;           // 选择IWDG外设
    IWDG_Handler.Init.Prescaler = prescaler;// 设置分频系数
    IWDG_Handler.Init.Reload = reload;      // 设置自动重装载值
    HAL_IWDG_Init(&IWDG_Handler);           // 初始化IWDG
}

void IWDG_Feed(void)
{
    HAL_IWDG_Refresh(&IWDG_Handler);        // 喂狗
}
