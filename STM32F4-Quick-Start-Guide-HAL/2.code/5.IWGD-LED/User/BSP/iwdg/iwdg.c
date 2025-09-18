#include "iwdg.h"

IWDG_HandleTypeDef IWDG_Handler;
// time = ((4*2^pre) * reload) / 40 (ms)
void bsp_iwdg_init(uint8_t prescaler, uint16_t reload)
{
    IWDG_Handler.Instance = IWDG;           // 选择IWDG外设
    IWDG_Handler.Init.Prescaler = prescaler;// 设置分频系数
    IWDG_Handler.Init.Reload = reload;      // 设置自动重装载值
    HAL_IWDG_Init(&IWDG_Handler);           // 初始化IWDG
}

void iwdg_feed(void)
{
    HAL_IWDG_Refresh(&IWDG_Handler);        // 喂狗
}
