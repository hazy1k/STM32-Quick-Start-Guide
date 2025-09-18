# 第三章 EXTI介绍及应用

## 1. NVIC和EXTI简介

### 1.1 NVIC简介

什么是 NVIC？ NVIC 即嵌套向量中断控制器，全称 Nested vectored interrupt controller。它是内核的器件，所以它的更多描述可以看内核有关的资料。 M3/M4/M7 内核都是支持 256 个中断，其中包含了 16 个系统中断和 240 个外部中断，并且具有 256 级的可编程中断设置。 然而芯片厂商一般不会把内核的这些资源全部用完，如 STM32F407 的系统中断有 10 个，外部中断有82 个。

STM32 中的中断优先级可以分为：抢占式优先级和响应优先级，响应优先级也称子优先级，每个中断源都需要被指定这两种优先级。抢占式优先级和响应优先级的区别：

抢占优先级： 抢占优先级高的中断可以打断正在执行的抢占优先级低的中断。

响应优先级： 抢占优先级相同， 响应优先级高的中断不能打断响应优先级低的中断。

还有一种情况就是当两个或者多个中断的抢占式优先级和响应优先级相同时，那么就遵循自然优先级，看中断向量表的中断排序，数值越小，优先级越高。

### 1.2 EXTI简介

EXTI 即是外部中断和事件控制器，它是由 20 个产生事件/中断请求的边沿检测器组成。每一条输入线都可以独立地配置输入类型（脉冲或挂起）和对应的触发事件（上升沿或下降沿或者双边沿都触发）。每个输入线都可以独立地被屏蔽。挂起寄存器保持着状态线的中断请求。

产生中断线路目的使把输入信号输入到 NVIC，进一步运行中断服务函数，实现功能。而产生事件线路目的是传输一个脉冲信号给其他外设使用，属于硬件级功能。

EXTI 支持 23 个外部中断/事件请求，这些都是信息输入端，也就是上面提及到了输入线，具体如下：

EXTI 线 0~15：对应外部 IO 口的输入中断

EXTI 线 16：连接到 PVD 输出EXTI 线 17：连接到 RTC 闹钟事件EXTI 线 18：连接到 USB 唤醒事件EXTI 线 19：连接到以太网唤醒事件

EXTI 线 20：连接到 USB OTG HS（在 FS 中配置）唤醒事件EXTI 线 21：连接到 RTC 入侵和时间戳事件

EXTI 线 22：连接到 RTC 唤醒事件

从上面可以看出， STM32F407 供给 IO 口使用的中断线只有 16 个，但是 STM32F407 的 IO口却远远不止 16 个，所以 STM32 把 GPIO 管脚 GPIOx.0~GPIOx.15(x=A,B,C,D,E,F,G)分别对应中断线 0~15。这样子每个中断线对应了最多 7 个 IO 口，以线 0 为例：它对应了 GPIOA.0、GPIOB.0、 GPIOC.0、 GPIOD.0、 GPIOE.0、 GPIOF.0 和 GPIOG.0。而中断线每次只能连接到 1 个IO 口上，这样就需要通过配置决定对应的中断线配置到哪个 GPIO 上了。

## 2. EXTI应用示例

### 2.1 EXTI外部中断配置步骤

1. 使能IO口时钟

2. 设置IO口模式，触发条件，开启SYSCFG时钟，设置IO口与中断线的映射关系

3. 配置中断优先级，使能中断

4. 编写中断服务函数

5. 编写中断处理回调函数HAL_GPIO_EXTI_Callback

### 2.2 EXTI相关参数宏定义

```c
#ifndef __KEY_H__
#define __KEY_H__

#include "sys.h"
#include "delay.h"

/* 普通按键 */
#define KEY_GPIO_Port GPIOE
#define KEY_GPIO_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()
#define KEY0_GPIO_Pin GPIO_PIN_4 // KEY0 - PE4
#define KEY1_GPIO_Pin GPIO_PIN_3 // KEY1 - PE3
#define KEY2_GPIO_Pin GPIO_PIN_2 // KEY2 - PE2
/* UP按键 */
#define WKUP_GPIO_Port GPIOA // WKUP - PA0
#define WKUP_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define WKUP_GPIO_Pin GPIO_PIN_0

/* 中断处理 */
#define KEY0_INT_IRQn EXTI4_IRQn
#define KEY0_INT_IRQHandler EXTI4_IRQHandler
#define KEY1_INT_IRQn EXTI3_IRQn
#define KEY1_INT_IRQHandler EXTI3_IRQHandler
#define KEY2_INT_IRQn EXTI2_IRQn
#define KEY2_INT_IRQHandler EXTI2_IRQHandler
#define WKUP_INT_IRQn EXTI0_IRQn
#define WKUP_INT_IRQHandler EXTI0_IRQHandler

/* 读取按键状态 */
#define KEY0_GET HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY0_GPIO_Pin)
#define KEY1_GET HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY1_GPIO_Pin)
#define KEY2_GET HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY2_GPIO_Pin)
#define WKUP_GET HAL_GPIO_ReadPin(WKUP_GPIO_Port, WKUP_GPIO_Pin)

#define KEY0_Press 1 // KEY0 按键按下
#define KEY1_Press 2 // KEY1 按键按下
#define KEY2_Press 3 // KEY2 按键按下
#define WKUP_Press 4 // WKUP 按键按下

void bsp_key_init(void);
uint8_t key_scan(uint8_t mode);
void bsp_exti_init(void);

#endif /* __KEY_H__ */
```

### 2.3 初始化中断结构体

```c
void bsp_exti_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    bsp_key_init();
    /* KEY0 - PE4 - 下降沿触发中断 */
    GPIO_InitStructure.Pin = KEY0_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING; //  下降沿触发
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStructure);
    /* KEY1 - PE3 - 下降沿触发中断 */
    GPIO_InitStructure.Pin = KEY1_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStructure);
    /* KEY2 - PE2 - 下降沿触发中断 */
    GPIO_InitStructure.Pin = KEY2_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStructure);
    /* WKUP - PA0 - 上升沿触发中断 */
    GPIO_InitStructure.Pin = WKUP_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(WKUP_GPIO_Port, &GPIO_InitStructure);   
    /* NVIC配置 */
    HAL_NVIC_SetPriority(KEY0_INT_IRQn, 0, 2); // 抢占0，子优先级2
    HAL_NVIC_EnableIRQ(KEY0_INT_IRQn); // 使能EXTI_Line4
    HAL_NVIC_SetPriority(KEY1_INT_IRQn, 1, 2); // 抢占1，子优先级2 
    HAL_NVIC_EnableIRQ(KEY1_INT_IRQn); // 使能EXTI_Line3
    HAL_NVIC_SetPriority(KEY2_INT_IRQn, 2, 2); // 抢占2，子优先级2
    HAL_NVIC_EnableIRQ(KEY2_INT_IRQn); //  使能EXTI_Line2
    HAL_NVIC_SetPriority(WKUP_INT_IRQn, 3, 2); //  抢占3，子优先级2
    HAL_NVIC_EnableIRQ(WKUP_INT_IRQn); //  使能EXTI_Line0
}
```

### 2.4 中断服务函数

```c
// 中断服务函数
void KEY0_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY0_GPIO_Pin); // 清中断标志位
    __HAL_GPIO_EXTI_CLEAR_IT(KEY0_GPIO_Pin);
}
void KEY1_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY1_GPIO_Pin);
    __HAL_GPIO_EXTI_CLEAR_IT(KEY1_GPIO_Pin);
}
void KEY2_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY2_GPIO_Pin);
    __HAL_GPIO_EXTI_CLEAR_IT(KEY2_GPIO_Pin);
}
void WKUP_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(WKUP_GPIO_Pin);
    __HAL_GPIO_EXTI_CLEAR_IT(WKUP_GPIO_Pin);
}
```

### 2.5 中断回调函数

```c
// 中断回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
        case KEY0_GPIO_Pin:
            LED_TOGGLE(LED0_GPIO_Pin); 
            break;
        case KEY1_GPIO_Pin:
            LED_TOGGLE(LED1_GPIO_Pin);
            break;
        case KEY2_GPIO_Pin:
            BEEP_ON();            
            break;
        case WKUP_GPIO_Pin:
                BEEP_OFF();
            break;
        default:    
            break;
    } 
}
```

### 2.6 主函数测试

```c
#include "bsp_init.h"

int main(void)
{
    bsp_init();
    while(1)
    {
        delay_ms(1000);
    }
}
```

## 3. 常见EXTI函数（HAL库）

### 3.1 核心操作函数

**`void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)`**

- **功能:** 这是 **EXTI 中断的通用入口处理函数**。它**必须**被放置在对应 EXTI 线的中断服务函数 (`EXTIx_IRQHandler`) 中。

- **作用:**
  
  - 检查传入的 `GPIO_Pin` 对应的 EXTI 线的挂起标志位 (`EXTI->PR`) 是否被置位。
  
  - 如果置位，则**清除该 EXTI 线的挂起标志位** (通过写 1 到 `EXTI->PR` 的相应位)。
  
  - 调用弱定义的回调函数 `HAL_GPIO_EXTI_Callback(GPIO_Pin)`。

- **参数:**
  
  - `GPIO_Pin`: 触发中断的**引脚编号** (例如 `GPIO_PIN_0`, `GPIO_PIN_1`)。这个参数告诉函数需要检查和清除哪条 EXTI 线（例如 `GPIO_PIN_0` 对应 EXTI 线 0）。

```c
// 处理连接到 EXTI 线 0 的引脚 (如 PA0, PB0, PC0 等) 的中断
void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0); // 处理 PIN_0 对应的中断线 (EXTI0)
}
// 处理连接到 EXTI 线 1 的引脚 (如 PA1, PB1, PC1 等) 的中断
void EXTI1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1); // 处理 PIN_1 对应的中断线 (EXTI1)
}
// ... 类似处理 EXTI2, EXTI3
// 处理 EXTI 线 15 到 10 的引脚 (它们共享一个中断向量)
void EXTI15_10_IRQHandler(void)
{
  // 检查并处理所有可能的引脚 (10, 11, 12, 13, 14, 15)
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}
```

**`__weak void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)`**

- **功能:** 这是 **EXTI 中断的实际业务逻辑处理函数**。它是一个 **弱定义 (weak)** 的函数。`HAL_GPIO_EXTI_IRQHandler` 在确认中断源并清除标志位后，会调用这个函数。

- **关键:** **你必须在你的用户代码中 (例如 `main.c`, `gpio.c` 或自定义文件) 重新实现 (override) 这个函数**，并在里面编写处理该中断事件的具体逻辑。

- **参数:**
  
  - `GPIO_Pin`: 触发中断的**引脚编号**。这允许你在同一个回调函数中处理来自不同引脚的中断，通过检查 `GPIO_Pin` 参数来区分来源。

```c
// 用户实现的 EXTI 回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  // 判断是哪个引脚触发的中断
  if (GPIO_Pin == USER_BUTTON_PIN) // USER_BUTTON_PIN 是你定义的按键宏 (例如 GPIO_PIN_0)
  {
    // 处理按键按下事件 (例如翻转 LED)
    HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
    // 注意: 这里通常只是设置标志位或执行简单快速的操作！
  }
  // 可以添加其他引脚的中断处理...
  if (GPIO_Pin == SENSOR_INTERRUPT_PIN)
  {
    // 处理传感器中断事件 (例如读取数据)
    // ...
  }
}
```

- **重要原则:**
  
  - **保持简短快速！** 不要在回调函数中执行耗时操作（如 `HAL_Delay()`, 复杂的计算, 阻塞的通信）。中断处理应尽快完成。
  
  - **常用模式:** 在回调函数内部设置一个 `volatile` 全局标志位 (`flagButtonPressed = 1;`)，然后在 `main` 函数的超级循环 (`while (1)`) 中检查这个标志位并执行实际的耗时任务。这称为“中断+轮询”或“前后台系统”模式。

### 3.2 流程操作

1. **GPIO 模式设置:** 在初始化 GPIO 时 (`HAL_GPIO_Init`)，将 `GPIO_InitStruct.Mode` 设置为以下之一：
   
   - `GPIO_MODE_IT_RISING`: 上升沿触发中断
   
   - `GPIO_MODE_IT_FALLING`: 下降沿触发中断
   
   - `GPIO_MODE_IT_RISING_FALLING`: 上升沿和下降沿都触发中断
   
   - `GPIO_MODE_EVT_*`: 事件模式 (不进入中断，用于唤醒睡眠模式，这里不讨论)

2. **开启 GPIO 时钟:** 确保对应 GPIO 端口的时钟已开启 (`__HAL_RCC_GPIOx_CLK_ENABLE()`)。

3. **配置 SYSCFG (关键!):** STM32F4 中，需要配置 SYSCFG 外设将具体的 **GPIO 引脚** 连接到对应的 **EXTI 线**。
   
   - **函数:** `void HAL_SYSCFG_EXTILineConfig(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex)`
   
   - **作用:** 将 `GPIOx` 的 `Pin x` 连接到 `EXTI Line x`。
   
   - **参数:**
     
     - `EXTI_PortSourceGPIOx`: 指定 GPIO 端口源 (例如 `EXTI_PORTSOURCE_GPIOA`, `EXTI_PORTSOURCE_GPIOB` ...)。
     
     - `EXTI_PinSourcex`: 指定 GPIO 引脚源 (例如 `EXTI_PINSOURCE_0`, `EXTI_PINSOURCE_1` ... `EXTI_PINSOURCE_15`)。这个数字 `x` 决定了连接到哪条 EXTI 线（EXTI0, EXTI1, ... EXTI15）。
   
   - **位置:** 通常在 `HAL_GPIO_Init` **之后** 调用。
   
   - **示例:** 配置 PA0 连接到 EXTI 线 0

```c
HAL_SYSCFG_EXTILineConfig(EXTI_PORTSOURCE_GPIOA, EXTI_PINSOURCE_0);
```

4. **配置 NVIC (嵌套向量中断控制器):**
- **作用:** 设置 EXTI 线的中断优先级并启用中断。

- **函数:**
  
  - `HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)`: 设置中断优先级。
  
  - `HAL_NVIC_EnableIRQ(IRQn_Type IRQn)`: 使能中断。

- **参数 `IRQn`:** 指定要配置的 EXTI 中断线对应的 IRQ 号。不同 EXTI 线有不同的 IRQn：
  
  - `EXTI0_IRQn`: EXTI 线 0
  
  - `EXTI1_IRQn`: EXTI 线 1
  
  - `EXTI2_IRQn`: EXTI 线 2
  
  - `EXTI3_IRQn`: EXTI 线 3
  
  - `EXTI4_IRQn`: EXTI 线 4
  
  - `EXTI9_5_IRQn`: EXTI 线 5 到 9 (共享一个中断向量)
  
  - `EXTI15_10_IRQn`: EXTI 线 10 到 15 (共享一个中断向量)

- **示例 (配置 EXTI 线 0):**

```c
// 设置 EXTI0 中断优先级 (抢占优先级 0, 子优先级 0)
HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
// 使能 EXTI0 中断
HAL_NVIC_EnableIRQ(EXTI0_IRQn);
```

5. **编写中断服务函数 (ISR):** 如上所述，在 `stm32f4xx_it.c` 中实现对应的 `EXTIx_IRQHandler` 函数，并在其中调用 `HAL_GPIO_EXTI_IRQHandler(GPIO_Pin)`。

6. **实现回调函数:** 在你的用户代码中 (如 `main.c`) **重新实现** `HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)` 函数，并在其中根据 `GPIO_Pin` 参数编写具体的中断处理逻辑。
