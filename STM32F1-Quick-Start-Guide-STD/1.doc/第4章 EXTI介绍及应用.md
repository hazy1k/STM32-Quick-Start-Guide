# 第四章 EXTI介绍及应用

## 1. EXTI介绍

EXTI（External interrupt/event controller）—外部中断/事件控制器，管理了控制器的 20 个中断/事件线。每个中断/事件线都对应有一个边沿检测器，可以实现输入信号的上升沿检测和下降沿的检测。 EXTI 可以实现对每个中断/事件线进行单独配置，可以单独配置为中断或者事件，以及触发事件的属性。

![屏幕截图 2025-05-29 104923.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/05/29-10-49-39-屏幕截图%202025-05-29%20104923.png)

EXTI 有 20 个中断/事件线，每个 GPIO 都可以被设置为输入线，占用 EXTI0 至 EXTI15，还有另外七根用于特定的外设事件

![屏幕截图 2025-05-29 105234.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/05/29-10-52-40-屏幕截图%202025-05-29%20105234.png)

EXTI0 至 EXTI15 用于 GPIO，通过编程控制可以实现任意一个 GPIO 作为 EXTI 的输入源。

## 2. EXTI初始化结构体介绍

标准库函数对每个外设都建立了一个初始化结构体，比如 EXTI_InitTypeDef，结构体成员用于设置外设工作参数，并由外设初始化配置函数，比如 EXTI_Init() 调用，这些设定参数将会设置外设相应的寄存器，达到配置外设工作环境的目的。

初始化结构体和初始化库函数配合使用是标准库精髓所在，理解了初始化结构体每个成员意义基本上就可以对该外设运用自如了。初始化结构体定义在 stm32f10x_exti.h 文件中，初始化库函数定义在 stm32f10x_exti.c 文件中，编程时我们可以结合这两个文件内注释使用。

```c
typedef struct {
    uint32_t EXTI_Line; // 中断/事件线
    EXTIMode_TypeDef EXTI_Mode; // EXTI 模式
    EXTITrigger_TypeDef EXTI_Trigger; // 触发类型
    FunctionalState EXTI_LineCmd; // EXTI 使能
} EXTI_InitTypeDef;
```

1. EXTI_Line： EXTI 中断/事件线选择，可选 EXTI0 至 EXTI19

2. EXTI_Mode： EXTI 模式选择，可选为产生中断 (EXTI_Mode_Interrupt) 或者产生事件(EXTI_Mode_Event)。

3. EXTI_Trigger： EXTI 边沿触发事件，可选上升沿触发 (EXTI_Trigger_Rising)、下降沿触发 (EXTI_Trigger_Falling) 或者上升沿和下降沿都触发 ( EXTI_Trigger_Rising_Falling)。

4. EXTI_LineCmd：控制是否使能 EXTI 线，可选使能 EXTI 线 (ENABLE) 或禁用 (DISABLE)。

## 3. EXTI应用示例

轻触按键在按下时会使得引脚接通，通过电路设计可以使得按下时产生电平变化

![屏幕截图 2025-05-29 105914.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/05/29-10-59-18-屏幕截图%202025-05-29%20105914.png)

### 3.1 KEY和EXTI相关参数宏定义

```c
#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"

/* KEY GPIO */
#define KEY1_GPIO_CLK RCC_APB2Periph_GPIOA // KEY1 PA0
#define KEY1_GPIO_Port GPIOA
#define KEY1_GPIO_Pin GPIO_Pin_0
#define KEY2_GPIO_CLK RCC_APB2Periph_GPIOC // KEY2 PC13
#define KEY2_GPIO_Port GPIOC
#define KEY2_GPIO_Pin GPIO_Pin_13

/* KEY EXTI */
#define KEY1_EXTI_CLK (RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO)
#define KEY1_EXTI_Port GPIO_PortSourceGPIOA // 外部中断端口
#define KEY1_EXTI_Pin GPIO_PinSource0 // 外部中断引脚
#define KEY1_EXTI_Line EXTI_Line0 // 外部中断线
#define KEY1_EXTI_IRQ EXTI0_IRQn // 外部中断中断号
#define KEY1_IRQHandler EXTI0_IRQHandler // 外部中断服务函数

#define KEY2_EXTI_CLK (RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO)
#define KEY2_EXTI_Port GPIO_PortSourceGPIOC
#define KEY2_EXTI_Pin GPIO_PinSource13
#define KEY2_EXTI_Line EXTI_Line13
#define KEY2_EXTI_IRQ EXTI15_10_IRQn
#define KEY2_IRQHandler EXTI15_10_IRQHandler

#define KEY_OFF 0
#define KEY_ON 1

void NVIC_Configuration(void);
void KEY_EXTI_Init(void);

#endif /* __KEY_H__ */

```

### 3.2 NVIC配置

```c
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 配置中断优先级组
    NVIC_InitStructure.NVIC_IRQChannel = KEY1_EXTI_IRQ; // 配置中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 配置中断优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 配置中断子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = KEY2_EXTI_IRQ;
    NVIC_Init(&NVIC_InitStructure);
}
```

### 3.3 KEY GPIO配置及EXTI

```c
void KEY_EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    RCC_APB2PeriphClockCmd(KEY1_EXTI_CLK|KEY2_EXTI_CLK, ENABLE);
    NVIC_Configuration();
    /* GPIO 配置 */
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_Pin;
    GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStructure);
    /* EXTI 配置 */
    GPIO_EXTILineConfig(KEY1_EXTI_Port, KEY1_EXTI_Pin); // 配置中断引脚
    EXTI_InitStructure.EXTI_Line = KEY1_EXTI_Line; // 配置中断线
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; // 配置中断模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE; // 使能中断线
    EXTI_Init(&EXTI_InitStructure);
    GPIO_EXTILineConfig(KEY2_EXTI_Port, KEY2_EXTI_Pin);
    EXTI_InitStructure.EXTI_Line = KEY2_EXTI_Line;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}
```

### 3.4 中断服务函数

```c
void KEY1_IRQHandler(void)
{
  if(EXTI_GetITStatus(KEY1_EXTI_Line) != RESET)
  {
    LED_TOGGLE(RED_LED_Pin);
    EXTI_ClearITPendingBit(KEY1_EXTI_Line); // 清中断标志位
  }
}

void KEY2_IRQHandler(void)
{
  if(EXTI_GetITStatus(KEY2_EXTI_Line) != RESET)
  {
    LED_TOGGLE(BLUE_LED_Pin);
    EXTI_ClearITPendingBit(KEY2_EXTI_Line); // 清中断标志位
  }
}
```

### 3.5 主函数测试

```c
#include "stm32f10x.h"
#include "led.h"
#include "key.h"

int main()
{
	BSP_LED_Init();
	KEY_EXTI_Init();
	while(1);
}

```

## 4. EXTI常见函数

### 4.1 基本配置步骤

- 使能时钟

```c
// 使能AFIO时钟（EXTI配置需要）
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
```

- **配置GPIO为输入模式**

```c
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;         // 以PA0为例
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;     // 上拉输入模式
GPIO_Init(GPIOA, &GPIO_InitStruct);
```

- **映射GPIO到EXTI线**

```c
// 将PA0映射到EXTI0线
GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
```

- **配置EXTI线**

```c
EXTI_InitTypeDef EXTI_InitStruct;
EXTI_InitStruct.EXTI_Line = EXTI_Line0;        // 选择EXTI0线
EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt; // 中断模式
EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿触发
EXTI_InitStruct.EXTI_LineCmd = ENABLE;         // 使能该线
EXTI_Init(&EXTI_InitStruct);
```

- **配置NVIC（嵌套向量中断控制器）**

```c
NVIC_InitTypeDef NVIC_InitStruct;
NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;  // EXTI0中断通道
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x0F; // 抢占优先级
NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0F;      // 子优先级
NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;   // 使能通道
NVIC_Init(&NVIC_InitStruct);
```

### 4.2 关键EXTI操作函数

- 初始化与配置

```c
void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct);
```

- 参数结构体

```c
typedef struct {
  uint32_t EXTI_Line;    // 选择EXTI线 (EXTI_Line0~EXTI_Line15)
  EXTIMode_TypeDef EXTI_Mode;       // 模式: EXTI_Mode_Interrupt 或 EXTI_Mode_Event
  EXTITrigger_TypeDef EXTI_Trigger; // 触发: EXTI_Trigger_Rising/Falling/Both
  FunctionalState EXTI_LineCmd;     // 使能状态: ENABLE/DISABLE
} EXTI_InitTypeDef;
```

- **GPIO引脚映射到EXTI线**

```c
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
```

- 中断状态管理

```c
// 检查中断标志：
ITStatus EXTI_GetITStatus(uint32_t EXTI_Line);
// 清除中断挂起位
void EXTI_ClearITPendingBit(uint32_t EXTI_Line);
```

- **软件触发中断**

```c
void EXTI_GenerateSWInterrupt(uint32_t EXTI_Line);
```

### 4.3 中断服务函数

- 单个引脚中断（EXTI0~EXTI14）

```c
// PA0~PA4等引脚的中断服务函数
void EXTI0_IRQHandler(void) {
  if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
    // 处理中断
    EXTI_ClearITPendingBit(EXTI_Line0); // 清除标志位
  }
}
```

- **多引脚共用中断**

```c
// EXTI5~9共用中断服务函数
void EXTI9_5_IRQHandler(void) {
  if (EXTI_GetITStatus(EXTI_Line5) != RESET) {
    // 处理EXTI5中断
    EXTI_ClearITPendingBit(EXTI_Line5);
  }
  if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
    // 处理EXTI6中断
    EXTI_ClearITPendingBit(EXTI_Line6);
  }
  // ... 其他线同理
}

// EXTI10~15共用中断服务函数
void EXTI15_10_IRQHandler(void) {
  // 处理方式同上
}
```
