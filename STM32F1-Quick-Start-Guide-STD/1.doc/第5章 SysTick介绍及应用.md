# 第五章 SysTick介绍及应用

## 1. SysTick简介

SysTick—系统定时器是属于 CM3 内核中的一个外设，内嵌在 NVIC 中。系统定时器是一个 24bit的向下递减的计数器，计数器每计数一次的时间为 1/SYSCLK，一般我们设置系统时钟 SYSCLK等于 72M。当重装载数值寄存器的值递减到 0 的时候，系统定时器就产生一次中断，以此循环往复。

因为 SysTick 是属于 CM3 内核的外设，所以所有基于 CM3 内核的单片机都具有这个系统定时器，使得软件在 CM3 单片机中可以很容易的移植。系统定时器一般用于操作系统，用于产生时基，维持操作系统的心跳。

## 2. SysTick应用示例

### 2.1 SysTick中断配置及服务函数

```c
void SysTick_Init(void)
{
    NVIC_SetPriority(SysTick_IRQn, 0x00); // 设置延时级最高
    if(SysTick_Config(SystemCoreClock / 100000)) // 10us中断一次
    {
        while(1);
    }
}

void SysTick_Handler(void)
{
    if(Delay_Time != 0)
    {
        Delay_Time--; // 每次SysTick中断减1
    }
}
```

### 2.2 延时函数

```c
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
```

### 2.3 主函数测试

```c
#include "stm32f10x.h"
#include "led.h"
#include "SysTick.h"

int main()
{
	BSP_LED_Init();
	SysTick_Init();
	while(1)
	{
		LED_ON(RED_LED_Pin);
		Delay_ms(1000);
		LED_OFF(RED_LED_Pin);
		Delay_ms(1000);
		LED_ON(GREEN_LED_Pin);
		Delay_ms(1000);
		LED_OFF(GREEN_LED_Pin);
		Delay_ms(1000);
	}
}

```

## 3. SysTick常见函数（STD库）

### 3.1 核心操作函数

#### 3.1.1 初始化SysTick

```c
uint32_t SysTick_Config(uint32_t ticks);
```

- **功能**：配置 SysTick 重装载值并启动定时器

- **参数**：`ticks` - 两次中断之间的时钟周期数

- **返回值**：
  
  - 0：成功
  
  - 1：参数无效（ticks > 0xFFFFFF）

- **示例**：

```c
// 系统时钟72MHz时，配置1ms中断
if (SysTick_Config(SystemCoreClock / 1000)) {
  // 初始化失败处理
}
```

#### 3.1.2 关闭SysTick

```c
void SysTick->CTRL = 0;  // 直接操作寄存器关闭
```

### 3.2 寄存器级操作

#### 3.2.1 手动配置SysTick

```c
// 设置重装载值（24位）
SysTick->LOAD = reload_value - 1;  // 注意：实际计数值=LOAD+1

// 清除当前计数值
SysTick->VAL = 0;

// 配置控制寄存器
SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |  // 选择时钟源（0=外部时钟，1=内核时钟）
                SysTick_CTRL_TICKINT_Msk   |  // 使能中断
                SysTick_CTRL_ENABLE_Msk;     // 启动定时器
```

#### 3.2.2 状态检测

```c
// 检查是否计数到0
if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
    // 计数到0时该标志位置1
}
```

### 3.3 常用时钟源配置

```c
// 选择时钟源（在SysTick_Config之前调用）
SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);     // 使用系统时钟（72MHz）
SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); // 使用系统时钟/8（9MHz）
```

### 3.4 精确延时实现

#### 3.4.1 阻塞式微秒延时

```c
void delay_us(uint32_t us) {
    SysTick->LOAD = SystemCoreClock/8000000 * us - 1; // 以9MHz工作
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    SysTick->CTRL = 0;
}
```

#### 3.4.2 阻塞式毫秒延时

```c
void delay_ms(uint32_t ms) {
    while (ms--) {
        delay_us(1000);
    }
}
```

### 3.5 中断服务函数

```c
volatile uint32_t sys_tick_count = 0;

void SysTick_Handler(void) {
    sys_tick_count++;  // 每中断一次计数+1
}
```
