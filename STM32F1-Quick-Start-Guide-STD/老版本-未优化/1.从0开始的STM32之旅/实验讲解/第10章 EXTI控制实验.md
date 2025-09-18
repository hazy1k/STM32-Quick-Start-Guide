# 第十章 EXTI控制实验

## 1. 硬件设计

    轻触按键在按下时会使得引脚接通，通过电路设计可以使得按下时产生电平变化

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/EXTI004.png)

## 2. 软件设计

### 2.1 编程大纲

1. 配置EXTI初始化

2. 配置NVIC初始化

3. 编写中断函数

4. 主函数应用

### 2.2 代码分析

#### 2.2.1 初始化用来产生中断的GPIO，初始化EXTI

```c
#ifndef __EXTI_H
#define __EXTI_H

#include "stm32f10x.h"

#define KEY1_GPIO_PORT GPIOA
#define KEY1_GPIO_PIN GPIO_Pin_0
#define KEY1_GPIO_CLK (RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO) // 使能GPIOA时钟和复用功能时钟
#define KEY1_EXTI_PORTSOURCE GPIO_PortSourceGPIOA // 外部中断端口
#define KEY1_EXTI_PINSOURCE GPIO_PinSource0 // 外部中断引脚
#define KEY1_EXTI_LINE EXTI_Line0 // 外部中断线
#define KEY1_EXTI_IRQ EXTI0_IRQn // 外部中断中断向量号
#define KEY1_IRQHandler EXTI0_IRQHandler // 外部中断服务函数

#define KEY2_GPIO_PORT GPIOC
#define KEY2_GPIO_PIN GPIO_Pin_13
#define KEY2_GPIO_CLK (RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO)
#define KEY2_EXTI_PORTSOURCE GPIO_PortSourceGPIOC 
#define KEY2_EXTI_PINSOURCE GPIO_PinSource13 
#define KEY2_EXTI_LINE EXTI_Line13 
#define KEY2_EXTI_IRQ EXTI15_10_IRQn 
#define KEY2_IRQHandler EXTI15_10_IRQHandler 

void NVIC_Configuration(void);
void EXTI_KeyInit(void);

#endif
```

在上面的宏定义中，我们除了开GPIO的端口时钟外，我们还打开了AFIO的时钟， 这是因为等下配置EXTI信号源的时候需要用到AFIO的外部中断控制寄存器AFIO_EXTICRx。

#### 2.2.2 配置NVIC

```c
// 首先配置NVIC
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 配置优先组
  NVIC_InitStructure.NVIC_IRQChannel = KEY1_EXTI_IRQ; // 配置中断源
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 配置抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 配置子优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断
  NVIC_Init(&NVIC_InitStructure); // 初始化

  // 配置中断源2即KEY2
  NVIC_InitStructure.NVIC_IRQChannel = KEY2_EXTI_IRQ; // 配置中断源
  NVIC_Init(&NVIC_InitStructure); // 沿用上面的配置
}
```

    这里我们配置两个的中断软件优先级一样，如果出现了两个按键同时按下的情况，那怎么办，到底该执行哪一个中断？当两个中断的软件优先级一样的时候， 中断来临时，具体先执行哪个中断服务函数由硬件的中断编号决定，编号越小，优先级越高。

    当然，我们也可以把抢占优先级设置成一样，子优先级设置成不一样，这样就可以区别两个按键同时按下的情况，而不用硬件去对比硬件编号。

#### 2.2.3 编写中断服务函数

```c
// 编写我们的中断函数
// 产生中断后，执行中断服务程序，处理中断源产生的事件。
void KEY1_IRQHandler(void)
{
  if(EXTI_GetITStatus(KEY1_EXTI_LINE) != RESET) // 首先确保EXTI确实产生了中断
  {
    LED1_TOGGLE(); // LED1 反转
    EXTI_ClearITPendingBit(KEY1_EXTI_LINE); // 因为事件已经处理完毕，清除中断标志位
  }
}

void KEY2_IRQHandler(void)
{
  if(EXTI_GetITStatus(KEY2_EXTI_LINE) != RESET)
  {
    LED2_TOGGLE();
    EXTI_ClearITPendingBit(KEY2_EXTI_LINE);
  }
}
```

照例我们得分析一下新出现的库函数：

`EXTI_GetITStatus`

- **功能**: 该函数用于检查指定的外部中断线的中断状态。

- **原型**: `ITStatus EXTI_GetITStatus(uint32_t EXTI_Line);`

- **参数**: `EXTI_Line` 是要检查的外部中断线的标识符，通常是一个位掩码。

- **返回值**: 返回 `SET` 或 `RESET`。如果中断发生，则返回 `SET`，否则返回 `RESET`。
  
  `EXTI_ClearITPendingBit`

- **功能**: 清除指定外部中断线的中断挂起标志位，以便下一次中断可以正常触发。

- **原型**: `void EXTI_ClearITPendingBit(uint32_t EXTI_Line);`

- **参数**: `EXTI_Line` 是要清除中断挂起标志位的外部中断线标识符。

- **返回值**: 无返回值。

#### 2.2.4 EXTI 初始化

```c
// EXTI初始化
void EXTI_KeyInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK, ENABLE);
  NVIC_Configuration(); // 配置NVIC
  // 配置GPIO
  GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入模式
  GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
  // 配置EXTI
  GPIO_EXTILineConfig(KEY1_EXTI_PORTSOURCE, KEY1_EXTI_PINSOURCE);
  EXTI_InitStructure.EXTI_Line = KEY1_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; // 中断模式
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; // 使能中断线
  EXTI_Init(&EXTI_InitStructure); // 初始化

  GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
  GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
  GPIO_EXTILineConfig(KEY2_EXTI_PORTSOURCE, KEY2_EXTI_PINSOURCE);
  EXTI_InitStructure.EXTI_Line = KEY2_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下升沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
  EXTI_Init(&EXTI_InitStructure);
}
```

介绍一下库函数：void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);

函数参数

1. **GPIO_PortSource**  
   该参数指定连接到外部中断（EXTI）线的 GPIO 端口。STM32 允许将每个 EXTI 线连接到特定的 GPIO 端口（如 GPIOA、GPIOB 等）。  
   例如，如果你想将 PA0 引脚连接到 EXTI0 线，`GPIO_PortSource` 就是 `GPIO_PortSourceGPIOA`。

2. **GPIO_PinSource**  
   该参数指定连接到 EXTI 中断线的 GPIO 引脚号。STM32 的每个 GPIO 端口（如 GPIOA、GPIOB）都有多个引脚（如 PA0、PA1、PB0、PB1 等）。你可以将某个引脚映射到某条 EXTI 线。  
   例如，如果你想将 PA0 引脚连接到 EXTI0 线，`GPIO_PinSource` 就是 `GPIO_PinSource0`。

## 3. 小结

我们代码注释已经写得足够清楚了，复盘一下大概流程吧：

首先是确立我们的目标，用key产生中断来控制LED：

- 使用两个及其以上的中断，NVIC是必不可少的，因为他是用来配置中断优先级的，当两个中断同时发生了怎么办，这个时候NVIC就出来了，五件套来配置：优先组、中断源、抢占优先级、子优先级、最后使能中断

- 后面肯定是要宏定义配置GPIO和EXTI初始化了，就拿key1来说，首先肯定是打开GPIO对应的时钟，然后是选择Pin、Mode，初始化

- EXTI配置也不用慌： 首先用这个库函数GPIO_EXTILineConfig()上面我们也讲解了，后面就是选择Line、Mode、Trigger、最后就是使能和初始化啦

- 配置完后，我们就来编写中断服务函数，我们举了一个简单的例子就是反转LED状态

达成的效果：当我们按下按键->产生中断->执行中断服务函数（led状态反转）->清除中断位->程序回顾正常

---

2024.8.21 第一次修订，后期不再维护

2024.12.20 丰富内容，优化代码
