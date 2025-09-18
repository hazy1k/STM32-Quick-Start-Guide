# 第九章 SysTick系统定时器

## 1. 导入

    SysTick—系统定时器是属于CM3内核中的一个外设，内嵌在NVIC中。系统定时器是一个24bit的向下递减的计数器， 计数器每计数一次的时间为1/SYSCLK，一般我们设置系统时钟SYSCLK等于72M。当重装载数值寄存器的值递减到0的时候，系统定时器就产生一次中断，以此循环往复。

    因为SysTick是属于CM3内核的外设，所以所有基于CM3内核的单片机都具有这个系统定时器，使得软件在CM3单片机中可以很容易的移植。 系统定时器一般用于操作系统，用于产生时基，维持操作系统的心跳。

    SysTick 定时器有以下基本特点：

- **计数范围：** 24 位计数器，最大计数值为 `0xFFFFFF`。
- **计时源：** 可以选择内部时钟或外部时钟作为计数源。
- **定时模式：** 配置为定时器工作模式，计数到零时会产生中断。
- **预分频：** 可以选择定时器的时钟源和预分频器来控制计时精度。

## 2. SysTick寄存器

    SysTick—系统定时器有4个寄存器，简要介绍如下。在使用SysTick产生定时的时候，只需要配置前三个寄存器，最后一个校准寄存器不需要使用。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic01.png)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic02.png)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic03.png)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic04.png)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic05.png)

假设我们希望将 SysTick 定时器配置为每 1 毫秒触发一次中断，以下是一个简单的配置示例：

```c
#include "stm32f4xx.h"  
void SysTick_Init(uint32_t ticks)
{
    // 设置 SysTick 重载值
    SysTick->LOAD = ticks - 1;  // 设置重载值，-1 是因为计数从 ticks 开始，最后为 0
    // 清除当前计数器值
    SysTick->VAL = 0;
    // 配置 SysTick 定时器时钟源（选择系统时钟作为时钟源）
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    // 使能 SysTick 中断
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    // 启动 SysTick 定时器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

int main(void)
{
    // 配置系统时钟
    SystemInit();  // STM32 默认时钟初始化，确保 SystemCoreClock 已经配置好
    // 假设系统时钟为 72 MHz，1 毫秒的 ticks 数量为 72,000
    uint32_t ticks = SystemCoreClock / 1000;  // 1 毫秒定时器
    // 初始化 SysTick 定时器
    SysTick_Init(ticks);
    while (1)
    {
        // 主循环
    }
}
// SysTick 中断处理函数
void SysTick_Handler(void)
{
    // 每当 SysTick 计数到 0 时，这个函数就会被调用
    // 在此处理定时任务，例如增加一个毫秒计数器等
}
```

---

2024.8.22 第一次修订，后期不再维护

2024.12.21 新增简单配置示例
