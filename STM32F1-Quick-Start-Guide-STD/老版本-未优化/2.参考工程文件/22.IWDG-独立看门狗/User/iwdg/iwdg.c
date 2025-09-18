#include "iwdg.h"

// 前置知识，iwdg超时时间计算
/*
	T = prv/40*rlv (s)
	prv为预分频器值，prv取值可以是[4,8,16,32,64,128,256]
	rlv为重载寄存器的值，取值范围为：0~0xFFF
	举例：T = 64/40*625 = 1s
*/
void IWDG_Config(uint8_t prv, uint16_t rlv)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); // 使能寄存器可写
	IWDG_SetPrescaler(prv); // 设置预分频器
	IWDG_SetReload(rlv); // 设置重载寄存器
	IWDG_Enable(); // 使能IWDG
}

void IWDG_Feed(void)
{
	IWDG_ReloadCounter(); // 重载计数器
}
