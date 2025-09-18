# 第八章 EXTI外部中断

## 1. EXTI简介

    EXTI（External interrupt/event controller）：外部中断/事件控制器，管理了控制器的20个中断/事件线。 每个中断/事件线都对应有一个边沿检测器，可以实现输入信号的上升沿检测和下降沿的检测。EXTI可以实现对每个中断/事件线进行单独配置， 可以单独配置为中断或者事件，以及触发事件的属性。

## 2. EXTI功能框图

    在图可以看到很多在信号线上打一个斜杠并标注“20”字样，这个表示在控制器内部类似的信号线路有20个， 这与EXTI总共有20个中断/事件线是吻合的。所以我们只要明白其中一个的原理，那其他19个线路原理也就知道了。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/EXTI002.png)

    EXTI可分为两大部分功能，一个是产生中断，另一个是产生事件，这两个功能从硬件上就有所不同。

### 2.1 产生中断

    首先我们来看图中红色虚线指示的电路流程。它是一个产生中断的线路，最终信号流入到NVIC控制器内。

    编号1是输入线，EXTI控制器有19个中断/事件输入线，这些输入线可以通过寄存器设置为任意一个GPIO，也可以是一些外设的事件， 这部分内容我们将在后面专门讲解。输入线一般是存在电平变化的信号。

    编号2是一个边沿检测电路，它会根据上升沿触发选择寄存器(EXTI_RTSR)和下降沿触发选择寄存器(EXTI_FTSR)对应位的设置来控制信号触发。 边沿检测电路以输入线作为信号输入端，如果检测到有边沿跳变就输出有效信号1给编号3电路，否则输出无效信号0。 而EXTI_RTSR和EXTI_FTSR两个寄存器可以控制需要检测哪些类型的电平跳变过程，可以是只有上升沿触发、只有下降沿触发或者上升沿和下降沿都触发。

    编号3电路实际就是一个或门电路，它的一个输入来自编号2电路，另外一个输入来自软件中断事件寄存器(EXTI_SWIER)。EXTI_SWIER允许我们通过程序控制就可以启动中断/事件线， 这在某些地方非常有用。我们知道或门的作用就是有1就为1，所以这两个输入随便一个有有效信号1就可以输出1给编号4和编号6电路。

    编号4电路是一个与门电路，它的一个输入是编号3电路，另外一个输入来自中断屏蔽寄存器(EXTI_IMR)。与门电路要求输入都为1才输出1， 导致的结果是如果EXTI_IMR设置为0时，那不管编号3电路的输出信号是1还是0，最终编号4电路输出的信号都为0；如果EXTI_IMR设置为1时， 最终编号4电路输出的信号才由编号3电路的输出信号决定，这样我们可以简单的控制EXTI_IMR来实现是否产生中断的目的。 编号4电路输出的信号会被保存到挂起寄存器(EXTI_PR)内，如果确定编号4电路输出为1就会把EXTI_PR对应位置1。

    编号5是将EXTI_PR寄存器内容输出到NVIC内，从而实现系统中断事件控制。

### 2.2 产生事件

    接下来我们来看看绿色虚线指示的电路流程。它是一个产生事件的线路，最终输出一个脉冲信号。

    产生事件线路是在编号3电路之后与中断线路有所不同，之前电路都是共用的。编号6电路是一个与门，它的一个输入来自编号3电路， 另外一个输入来自事件屏蔽寄存器(EXTI_EMR)。如果EXTI_EMR设置为0时，那不管编号3电路的输出信号是1还是0，最终编号6电路输出的信号都为0； 如果EXTI_EMR设置为1时，最终编号6电路输出的信号才由编号3电路的输出信号决定，这样我们可以简单的控制EXTI_EMR来实现是否产生事件的目的。

    编号7是一个脉冲发生器电路，当它的输入端，即编号6电路的输出端，是一个有效信号1时就会产生一个脉冲；如果输入端是无效信号就不会输出脉冲。

    编号8是一个脉冲信号，就是产生事件的线路最终的产物，这个脉冲信号可以给其他外设电路使用，比如定时器TIM、模拟数字转换器ADC等等，这样的脉冲信号一般用来触发TIM或者ADC开始转换。

    产生中断线路目的是把输入信号输入到NVIC，进一步会运行中断服务函数，实现功能，这样是软件级的。而产生事件线路目的就是传输一个脉冲信号给其他外设使用， 并且是电路级别的信号传输，属于硬件级的。

    另外，EXTI是在APB2总线上的，在编程时候需要注意到这点。

## 3. 中断/事件线

    EXTI有20个中断/事件线，每个GPIO都可以被设置为输入线，占用EXTI0至EXTI15， 还有另外七根用于特定的外设事件，见表

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/EXTI01.png)

    EXTI0至EXTI15用于GPIO，通过编程控制可以实现任意一个GPIO作为EXTI的输入源。由表可知， EXTI0可以通过AFIO的外部中断配置寄存器1(AFIO_EXTICR1)的EXTI0[3:0]位选择配置为PA0、 PB0、PC0、PD0、PE0、PF0、PG0、PH0或者PI0，见图。其他EXTI线(EXTI中断/事件线)使用配置都是类似的。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/EXTI003.png)

更多内容可以参考：[STM32 EXTI（外部中断） - 浇筑菜鸟 - 博客园](https://www.cnblogs.com/jzcn/p/15793353.html)

## 4. EXTI初始化结构体详解

   初始化结构体和初始化库函数配合使用是标准库精髓所在，理解了初始化结构体每个成员意义基本上就可以对该外设运用自如了。 初始化结构体定义在stm32f10x_exti.h文件中，初始化库函数定义在stm32f10x_exti.c文件中，编程时我们可以结合这两个文件内注释使用。

```c
// EXTI-1 EXTI初始化结构体
typedef struct {
    uint32_t EXTI_Line;                 // 中断/事件线
    EXTIMode_TypeDef EXTI_Mode;         // EXTI模式
    EXTITrigger_TypeDef EXTI_Trigger;   // 触发类型
    FunctionalState EXTI_LineCmd;       // EXTI使能
} EXTI_InitTypeDef;
```

### 4.1 `Line` 字段

- **作用**：指定要配置的外部中断线，通常对应于具体的 GPIO 引脚。
- **类型**：`uint32_t`
- **可选值**：
  - `EXTI_Line0` 到 `EXTI_Line15`（每个外部中断引脚对应一个 EXTI 中断线，例如 `EXTI_Line0` 对应 `PA0`，`EXTI_Line1` 对应 `PA1` 等）。

### 4.2 `Mode` 字段

- **作用**：指定中断线的工作模式（触发条件）。
- **类型**：`uint32_t`
- **可选值**：
  - `EXTI_Mode_Interrupt`：配置为中断模式，表示当触发条件满足时，产生中断请求。
  - `EXTI_Mode_Event`：配置为事件模式，表示当触发条件满足时，产生事件请求。事件不会生成中断，通常用于定时器输入捕获等应用。

### 4.3 `Trigger` 字段

- **作用**：配置触发中断的边沿条件。
- **类型**：`uint32_t`
- **可选值**：
  - `EXTI_Trigger_Rising`：触发条件为上升沿（从低电平到高电平）。
  - `EXTI_Trigger_Falling`：触发条件为下降沿（从高电平到低电平）。
  - `EXTI_Trigger_Rising_Falling`：触发条件为双边沿（既可以是上升沿，也可以是下降沿）。

### 4.4 `LineCmd` 字段

- **作用**：使能或禁用外部中断。
- **类型**：`FunctionalState`（可以是 `ENABLE` 或 `DISABLE`）
- **可选值**：
  - `ENABLE`：使能指定的外部中断线。
  - `DISABLE`：禁用指定的外部中断线。

以下是一个使用 `EXTI_InitTypeDef` 初始化外部中断的代码示例，假设我们要配置 `PA0` 引脚产生外部中断。

```c
#include "stm32f4xx_hal.h"

void EXTI0_IRQHandler(void)
{
    // 中断处理代码
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);  // 切换 GPIOB 的第 0 引脚状态
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);   // 清除中断标志
}

void EXTI_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStruct = {0};

    // 配置 PA0 产生中断
    EXTI_InitStruct.Line = EXTI_Line0;                 // 选择中断线0
    EXTI_InitStruct.Mode = EXTI_Mode_Interrupt;         // 配置为中断模式
    EXTI_InitStruct.Trigger = EXTI_Trigger_Rising;      // 上升沿触发
    EXTI_InitStruct.LineCmd = ENABLE;                   // 使能中断

    // 初始化 EXTI
    HAL_EXTI_SetConfigLine(&EXTI_InitStruct);
}

```

## 5. 小结

`EXTI_InitTypeDef` 结构体是配置 STM32 外部中断的核心部分，通过设置其各个字段，可以灵活配置不同引脚的中断触发方式和模式。一般来说，使用 EXTI 时，流程包括：

1. 配置 EXTI 引脚和中断触发方式。
2. 使能外部中断。
3. 配置 NVIC 使能中断服务函数。

---

2024.8.23 第一次修订，后期不再维护

2024.12.20 修补内容，新增“小结”章节
