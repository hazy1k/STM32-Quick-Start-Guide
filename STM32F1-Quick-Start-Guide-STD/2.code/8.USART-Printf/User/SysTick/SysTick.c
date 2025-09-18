#include "SysTick.h"
#include "core_cm3.h"
#include "misc.h"

__IO uint32_t Delay_Time = 0; // 延时计数器

void SysTick_Init(void)
{
    NVIC_SetPriority(SysTick_IRQn, 0x00); // 设置延时级最高
    if(SysTick_Config(SystemCoreClock / 100000)) // 10us中断一次
    {
        while(1);
    }
}

void Delay_10us(__IO uint32_t nTime)
{
    Delay_Time = nTime;
    while(Delay_Time != 0);
}

void Delay_us(__IO uint32_t us)
{
    Delay_10us(us / 10);
}

void Delay_ms(__IO uint32_t ms)
{
    Delay_us(ms * 1000);
}

void SysTick_Handler(void)
{
    if(Delay_Time != 0)
    {
        Delay_Time--; // 每次SysTick中断减1
    }
}
