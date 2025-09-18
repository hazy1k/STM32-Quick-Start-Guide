# 第十一章 SysTick定时实验

## 1. 硬件设计

    SysTick属于单片机内部的外设，不需要额外的硬件电路，剩下的只需一个LED灯即可。

## 2. 软件设计

### 2.1 编程大纲

1. 设置重装寄存器的值（10us）

2. 编写10us级延时函数、继续编写us级、ms级

3. 配置SysTick中断函数

4. 在主函数测试：流水灯每秒

### 2.2 代码分析

#### 2.2.1 SysTick初始化

```c
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
```

尽管注释简单说明了一下SysTick_Config()这个函数，但是我们还是要详细介绍一下：

##### 2.2.1.1 函数定义

```c
uint32_t SysTick_Config(uint32_t ticks);
```

##### 2.2.1.2 参数说明

- **`ticks`**：指定 SysTick 定时器的重载值（即计数的周期）。它决定了定时器中断的触发间隔。具体值为：

```c
ticks = SysTick 中断触发周期
```

这个值通常是系统时钟频率除以所需的定时中断频率。例如，如果你想要每 1 毫秒触发一次 SysTick 中断，而系统时钟频率为 72 MHz（72,000,000 Hz），那么：

```c
ticks = 72,000,000 / 1000 = 72000
```

这样，SysTick 定时器就会每 72,000 个时钟周期触发一次中断，也就是每毫秒触发一次。

##### 2.2.1.3 工作原理

SysTick 定时器是一个 24 位的递减计数器，计数从 `ticks` 开始，当计数值递减至 0 时，触发 SysTick 中断（如果已启用中断），然后重新加载计数器，继续从 `ticks` 重新开始。

##### 2.2.1.4 配置步骤

1. **设置 SysTick 计数器重载值**：将 `ticks` 值加载到 `SysTick->LOAD` 寄存器中。
2. **配置 SysTick 时钟源**：SysTick 的时钟源可以是系统时钟（`AHB`）或外部时钟。默认情况下，它使用 `AHB` 时钟。
3. **启用 SysTick 计数器**：启用计数器，使其开始递减。
4. **启用 SysTick 中断**：如果需要定时触发中断，可以启用中断，使其每次计数为 0 时触发中断。

#### 2.2.2 利用SysTick实现各级别延时

```c
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
```

#### 2.2.3 SysTick中断服务函数

```c
extern __IO uint32_t Delay_Time; // 引入 SysTick.c 中的延时计数器

void SysTick_Handler(void)
{
  if (Delay_Time != 0) 
  {
    Delay_Time--;  // 每次 SysTick 中断时，减少延时计数器
  }
}
```

#### 2.2.4 主函数

```c
#include "stm32f10x.h"
#include "SysTick.h"
#include "led.h"

int main()
{
	LED_Init();
	SysTick_Init();
	while(1)
	{
		LED1_ON();
		Delay_ms(1000);
		LED1_OFF();

		LED2_ON();
		Delay_ms(1000);
		LED2_OFF();

		LED3_ON();
		Delay_ms(1000);
		LED3_OFF();
	}
}

```

函数中初始化了LED和SysTick，然后在一个while循环中以1s的频率让LED闪烁

## 3. 小结

    这一章还是比较简单的，首先我们应该知道为什么要使用SysTick定时器-MCU的定时器资源有限，这样可以节省MCU资源，不用浪费一个定时器，点滴计时器就好像单片机的心脏一样，会根据固定的时间频率跳动。

    注意：

1. **SysTick 的计数器是 24 位**：即使系统时钟很高，计数器最多只能计数到 16,777,215。因此，如果配置的时间间隔超过了这个限制（大约 1 秒钟，具体取决于系统时钟），计数器将会溢出，需要重新加载。

2. **如果系统时钟变化**：如果你动态改变了系统时钟频率，需要重新配置 SysTick 定时器。

下面看一简单配置结束本章：

```c
#include "stm32f10x.h"

void SysTick_Handler(void)
{
    // 每当 SysTick 中断触发时，这个函数会被调用
    // 这里可以做延时相关操作
}

void SysTick_Init(void)
{
    // 配置 SysTick，每 1 毫秒触发一次中断
    if (SysTick_Config(SystemCoreClock / 1000))  // SystemCoreClock = 72MHz
    {
        // 配置失败时进入死循环
        while (1);
    }
}

int main(void)
{
    // 初始化 SysTick
    SysTick_Init();

    while (1)
    {
        // 主循环
        // 在这里进行其他操作
    }
}

```

---

2024.8.22 第一次修订，后期不再维护

2024.12.21 整体大改，代码优化
