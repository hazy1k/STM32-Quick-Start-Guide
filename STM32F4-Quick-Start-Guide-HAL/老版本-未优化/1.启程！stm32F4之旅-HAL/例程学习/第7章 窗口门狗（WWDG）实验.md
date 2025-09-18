# 第七章 窗口门狗（WWDG）实验

## 1. 硬件设计

本实验用到的硬件资源有：

- 指示灯DS0和DS1

- 窗口看门狗

其中指示灯前面介绍过了，窗口看门狗属于 STM32F4 的内部资源，只需要软件设置好即可正常工作。我们通过 DS0 和 DS1 来指示 STM32F4 的复位情况和窗口看门狗的喂狗情况。

## 2. 软件设计

### 2.1 编程大纲

1. 看门狗初始化

2. 看门狗配置和中断

3. 中断服务函数

### 2.2 代码分析

#### 2.2.1 看门狗初始化函数

```c
// 初始化窗口看门狗
/*
   参数：
   tr：T[6:0]，计数器值
   wr：W[6:0]，窗口值
   fp：分频系数，最低2位有效
   计算公式：
   Fwwdg = PCLK1/(4096*(2^fp)) ,其中PCLK1为时钟频率一般为42Mhz
*/
void WWDG_Init(uint8_t tr, uint8_t wr, uint32_t fp)
{
    WWDG_Handler.Instance = WWDG; // 选择看门狗外设
    WWDG_Handler.Init.Prescaler = fp; // 设置分频系数
    WWDG_Handler.Init.Window = wr; // 设置窗口值
    WWDG_Handler.Init.Counter = tr; // 设置计数器值
    WWDG_Handler.Init.EWIMode = WWDG_EWI_ENABLE; // 使能提前唤醒模式
    HAL_WWDG_Init(&WWDG_Handler); // 初始化看门狗
}
```

1. **`u8 tr`**: 计数器初值。WWDG计数器的起始值，值越大，看门狗重置的时间越长。
2. **`u8 wr`**: 窗口值。WWDG的窗口值，必须大于计数器值，用于监控计数器是否在规定范围内，以避免重置。
3. **`u32 fp`**: 分频系数。决定WWDG计数器的时钟频率。通常是一个预分频值，用于调整计数器的计数频率。

### 2.2 看门狗时钟及中断配置

```c
// 此函数会被WWDG_Init()调用
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
    __HAL_RCC_WWDG_CLK_ENABLE(); // 使能看门狗时钟
    HAL_NVIC_SetPriority(WWDG_IRQn, 2, 3); // 设置看门狗中断优先级
    HAL_NVIC_EnableIRQ(WWDG_IRQn); // 使能看门狗中断
}
```

1. **`__HAL_RCC_WWDG_CLK_ENABLE();`**: 使能WWDG外设的时钟。这是必须的步骤，以确保WWDG模块的正常工作。

2. **`HAL_NVIC_SetPriority(WWDG_IRQn,2,3);`**: 设置WWDG中断的优先级。`2`是抢占优先级，`3`是子优先级，这决定了中断处理的优先顺序。

3. **`HAL_NVIC_EnableIRQ(WWDG_IRQn);`**: 使能WWDG中断请求，确保中断能够被处理。

### 2.3 窗口看门狗中断服务函数

```c
// 看门狗中断服务函数
void WWDG_IRQHandler(void)
{
    HAL_WWDG_IRQHandler(&WWDG_Handler); // 调用WWDG共用中断处理函数
}

// 此函数会被HAL_WWDG_IRQHandler调用
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
    HAL_WWDG_Refresh(hwwdg); // 刷新看门狗计数器
    LED1 = !LED1;
}
```

### 2.4 主函数

```c
int main(void)
{
	HAL_Init();
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);
	uart_init(115200);
	KEY_Init();
	LED_Init();

	LED0_ON();
	delay_ms(300); // 延时300ms再启动看门狗

	// 窗口看门狗计算公式
	/*
		Fwwdg = PCLK1/(4096*(2^fp)) ,其中PCLK1为时钟频率一般为42Mhz
		函数参数：计数值、窗口值、分频系数
	*/
	WWDG_Init(0x7F, 0x5F, WWDG_PRESCALER_8); // 42Mhz/(4096*(2^8))=40Hz, 窗口值=0x5F, 计数值=0x7F
	while(1)
	{
		LED0_OFF();
	}
}
```

该函数通过 LED0来指示是否正在初始化。而 LED1(DS1)用来指示是否发生了中断。我们先让 LED0 亮 300ms，然后关闭以用于判断是否有复位发生了。在初始化 WWDG 之后，我们回到死循环，关闭 LED1，并等待看门狗中断的触发/复位。

## 3. 小结

我们再来简单复习一下WWDG吧：

1. **基本概念**:
   
   - WWDG会在设定的时间内进行计数。如果计数达到预设值而没有被“喂养”或重置，系统将会复位。
   - 窗口看门狗的“窗口”机制意味着在特定的时间范围内，重载计数器的操作必须在一个有效的时间区间内完成。

2. **窗口概念**:
   
   - 窗口看门狗有一个“窗口”时间。在这个窗口内，系统必须在每次计数周期内发送重载命令。
   - 如果在窗口外进行重载（即太早或太晚），则不会重置计数器，系统将会复位。

3. **应用流程**:
   
   - 系统启动后，WWDG开始计数。
   - 在计数器到达一定值之前，主程序需要在窗口期内重载WWDG。
   - 如果未能在窗口内重载，WWDG将触发复位。

示例代码如下：

```c
#include "main.h"
// 初始化WWDG
void WWDG_Init(void) {
    // 启用WWDG时钟
    __HAL_RCC_WWDG_CLK_ENABLE();
    // 配置WWDG
    WWDG_HandleTypeDef hwwdg;
    hwwdg.Instance = WWDG;
    hwwdg.Init.Prescaler = WWDG_PRESCALER_8;     // 设置预分频器
    hwwdg.Init.Window = 64;                      // 设置窗口大小
    hwwdg.Init.Counter = 127;                    // 设置计数器初始值
    if (HAL_WWDG_Init(&hwwdg) != HAL_OK) 
    {
        // 初始化错误处理
        Error_Handler();
    }
}
// 重载WWDG计数器
void WWDG_Refresh(void) 
{
    HAL_WWDG_Refresh(&hwwdg);
}
// 主函数
int main(void) {
    // 初始化HAL库
    HAL_Init();  
    // 系统时钟配置
    SystemClock_Config();
    // 初始化WWDG
    WWDG_Init();
    // 主循环
    while (1) 
    {
        // 执行其他任务
        // ...
        // 定期重载WWDG
        WWDG_Refresh();    
        // 小延迟，模拟其他操作
        HAL_Delay(100); // 根据需求调整延迟
    }
}
// 错误处理函数
void Error_Handler(void) 
{
    // 用户可以添加自己的错误处理代码
    while (1) {
        // 错误状态下的处理
    }
}
```

## 4. 独立与窗口看门狗同时工作

### 4.1 实验目标

1. 使用独立看门狗监控系统的总体健康状况。
2. 使用窗口看门狗监控关键任务的执行时间。
3. 在窗口看门狗即将超时前触发中断，并在中断服务程序中执行特定逻辑。

### 4.2 硬件需求

- STM32微控制器开发板
- LED（用于指示状态）
- 按钮（用于模拟故障）

### 4.3 实验步骤

1. **初始化看门狗**:
   
   - 配置独立看门狗（IWDG）。
   - 配置窗口看门狗（WWDG），并使能中断。

2. **主循环**:
   
   - 定期喂独立看门狗。
   - 在正常情况下，定期喂窗口看门狗。
   - 模拟故障时，停止喂狗。

3. **中断处理**:
   
   - 在窗口看门狗中断中，执行恢复操作，如重启某个任务或记录错误。

### 4.4 代码示例

以下是一个基于STM32 HAL库的代码示例：

```c
#include "stm32f4xx_hal.h"

IWDG_HandleTypeDef hiwdg;
WWDG_HandleTypeDef hwwdg;

// LED和按钮的引脚配置
#define LED_PIN GPIO_PIN_5
#define BUTTON_PIN GPIO_PIN_13

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_IWDG_Init(void);
static void MX_WWDG_Init(void);

void WWDG_IRQHandler(void)
{
    // 窗口看门狗中断处理
    HAL_WWDG_ClearFlag(&hwwdg);    
    // 执行你的处理逻辑，比如重启任务
    HAL_GPIO_TogglePin(GPIOA, LED_PIN); // 切换LED状态
}

int main(void) 
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_IWDG_Init();
    MX_WWDG_Init();
    while(1) 
    {
        // 喂独立看门狗
        HAL_IWDG_Refresh(&hiwdg);
        // 正常情况下喂窗口看门狗
        HAL_WWDG_Refresh(&hwwdg);
        // 模拟故障，例如按下按钮
        if(HAL_GPIO_ReadPin(GPIOC, BUTTON_PIN) == GPIO_PIN_RESET) 
        {
            // 不喂窗口看门狗，模拟故障
        }
        HAL_Delay(100); // 延迟以避免频繁喂狗
    }
}

static void MX_IWDG_Init(void) 
{
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_64; // 设置预分频器
    hiwdg.Init.Reload = 4095; // 设置重载值
    HAL_IWDG_Init(&hiwdg);
}

static void MX_WWDG_Init(void) 
{
    hwwdg.Instance = WWDG;
    hwwdg.Init.Prescaler = WWDG_PRESCALER_8; // 设置预分频器
    hwwdg.Init.Window = 80; // 设置窗口值
    hwwdg.Init.Counter = 127; // 设置初始计数值
    HAL_WWDG_Init(&hwwdg);
    HAL_NVIC_SetPriority(WWDG_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(WWDG_IRQn); // 使能中断
}

static void MX_GPIO_Init(void) 
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // LED引脚配置
    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 按钮引脚配置
    GPIO_InitStruct.Pin = BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void SystemClock_Config(void) 
{
    // 系统时钟配置代码
}
```

---

2024.9.30 第一次修订，后期不再维护

2025.1.12 修补内容，优化代码
