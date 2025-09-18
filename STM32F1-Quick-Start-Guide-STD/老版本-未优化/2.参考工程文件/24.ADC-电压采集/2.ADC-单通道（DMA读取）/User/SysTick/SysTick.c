#include "SysTick.h"
#include "core_cm3.h"
#include "misc.h"

// 前置知识-SysTick_Config库函数介绍
/*
    SysTick_Config 的作用是配置和启用 SysTick 定时器，
    使其以给定的 ticks 参数为周期，进行定时中断触发。
    输入参数：ticks，这个参数表示定时器的重载值
    返回值：如果成功配置 SysTick 定时器，则返回 0；否则返回 1。
*/

__IO uint32_t Delay_Time = 0; // 延时计数器

// SYSTick初始化
void SysTick_Init(void)
{
    NVIC_SetPriority(SysTick_IRQn, 0); // 设置为最高优先级，0是最高
    // 我们的时钟频率为72MHz，所有SysTick重装值为：
    // SystemFrequency / 100000	 10us中断一次
    if(SysTick_Config(SystemCoreClock / 100000))
    {
        // 如果SYStick配置失败，会返回1，会执行if语句中的代码
        while(1); // 死循环，等待复位
    }
}

// 利用SysTick实现10us级延时
void Delay_10us(__IO uint32_t nTime)
{
    Delay_Time = nTime;
    while(Delay_Time!= 0); // 等待延时计数器清零
}

// us级延时
void Delay_us(__IO uint32_t us)
{
    Delay_10us(us/10); // 转换为us级延时
}

// ms级延时
void Delay_ms(__IO uint32_t ms)
{
    Delay_us(ms*1000); // 转换为us级延时
}

void SysTick_Handler(void)
{
    if(Delay_Time != 0)
    {
        Delay_Time--; // 延时计数器减1
    }
}
