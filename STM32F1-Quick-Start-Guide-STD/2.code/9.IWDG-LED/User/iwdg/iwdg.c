#include "iwdg.h"

void IWDG_Init(uint8_t prv, uint16_t rld)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); // 允许写IWDG_PR和IWDG_RLD寄存器
    IWDG_SetPrescaler(prv); // 设置预分频器值
    IWDG_SetReload(rld); // 设置重装载值
    IWDG_ReloadCounter(); // 重新装载
    IWDG_Enable();
}

void IWDG_Feed(void)
{
    IWDG_ReloadCounter(); // 重新装载
}
