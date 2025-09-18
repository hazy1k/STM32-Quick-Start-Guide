# 第八章 WWDG介绍及应用

## 1. WWDG简介

STM32 有两个看门狗，一个是独立看门狗，一个是窗口看门狗。我们知道独立看门狗的工作原理就是一个递减计数器不断的往下递减计数，当减到 0 之前如果没有喂狗的话，产生复位。窗口看门狗跟独立看门狗一样，也是一个递减计数器不断的往下递减计数，当减到一个固定值 0X40时还不喂狗的话，产生复位，这个值叫窗口的下限，是固定的值，不能改变。这个是跟独立看门狗类似的地方，不同的地方是窗口看门狗的计数器的值在减到某一个数之前喂狗的话也会产生复位，这个值叫窗口的上限，上限值由用户独立设置。窗口看门狗计数器的值必须在上窗口和下窗口之间才可以喂狗，这就是窗口看门狗中窗口两个字的含义

![屏幕截图 2025-06-28 100437.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/06/28-10-04-48-屏幕截图%202025-06-28%20100437.png)

RLR 是重装载寄存器，用来设置独立看门狗的计数器的值。 TR 是窗口看门狗的计数器的值，由用户独立设置， WR 是窗口看门狗的上窗口值，由用户独立设置。

- **功能**：比IWDG更精确的看门狗，要求程序在特定"时间窗口"内喂狗

- **时钟源**：由APB1时钟分频（默认PCLK1 = 36MHz）

- **关键特性**：
  
  - 7位递减计数器（0x40~0x7F）
  
  - 可配置的"喂狗窗口"（0x40 ~ Window_Value）
  
  - 复位条件：
    
    - 计数器 < 0x40（超时未喂狗）
    
    - 计数器 > Window_Value时喂狗（过早喂狗）
  
  - 公式：**超时时间 = (4096 × 2^WDGTB × (T+1)) / PCLK1**

## 2. WWDG应用示例

WWDG 一般被用来监测，由外部干扰或不可预见的逻辑条件造成的应用程序背离正常的运行序列而产生的软件故障。比如一个程序段正常运行的时间是 50ms，在运行完这个段程序之后紧接着进行喂狗，如果在规定的时间窗口内还没有喂狗，那就说明我们监控的程序出故障了，跑飞了，那么就会产生系统复位，让程序重新运行。

### 2.1 WWDG基础配置

```c
#define WWDG_CNT 0x7F

static void WWDG_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// tr:递减计数器的值：取值范围0x7F~0x40
// wr:窗口值：取值范围0x7F~0x40
// prv:时钟预分频值
void WWDG_Init(uint8_t tr, uint8_t wr, uint32_t prv)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
    WWDG_SetCounter(tr);
    WWDG_SetWindowValue(wr); // 窗口值
    WWDG_SetPrescaler(prv); 
    WWDG_Enable(WWDG_CNT);
    WWDG_ClearFlag();
    WWDG_NVIC_Init();
    WWDG_EnableIT();
}
```

### 2.2 喂狗函数

```c
void WWDG_Feed(void)
{
    WWDG_SetCounter(WWDG_CNT);
}
```

### 2.3 主函数

```c
#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "wwdg.h"
#include "SysTick.h"
#include "usart.h"
#include <stdio.h>

int main(void) 
{
  SysTick_Init();
  BSP_LED_Init();
  BSP_KEY_Init();
  USARTx_Init();
  LED_ON(RED_LED_Pin);
  Delay_ms(1000);
  WWDG_Init(0x7F, 0x5F, WWDG_Prescaler_8); // 递减计数器初值：0x7F 窗口值：0x5F
  printf("wwdg reset\r\n"); 
  while(1)
  {
    LED_OFF(RED_LED_Pin);
    /*
    这部分应该为WWDG监控的程序代码
    计时器值，初始化成最大0X7F，当开启WWDG时候，这个值会不断减小
    当计数器的值大于窗口值时喂狗的话，会复位，当计数器减少到0X40
    还没有喂狗的话就非常非常危险了，计数器再减一次到了0X3F时就复位
    所以要当计数器的值在窗口值和0X40之间的时候喂狗，其中0X40是固定的
    */
    if(KEY_Scan(KEY1_GPIO_Port, KEY1_GPIO_Pin) == KEY_ON)
    {
      printf("wwdg feed\r\n");
      WWDG_Feed();
    }
  }
}
```

## 3. WWDG常见函数（STD库）

### 3.1 核心配置函数

#### 3.1.1 使能时钟

```c
RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
```

#### 3.1.2 设置预分频系数

```c
void WWDG_SetPrescaler(uint32_t WWDG_Prescaler);
```

**预分频系数表**：

| 宏定义                | 分频值 | 典型超时时间（PCLK1=36MHz） |
| ------------------ | --- | ------------------- |
| `WWDG_Prescaler_1` | 1   | 113μs ~ 7.28ms      |
| `WWDG_Prescaler_2` | 2   | 227μs ~ 14.56ms     |
| `WWDG_Prescaler_4` | 4   | 455μs ~ 29.12ms     |
| `WWDG_Prescaler_8` | 8   | 910μs ~ 58.25ms     |

#### 3.1.3 设置窗口值

```c
void WWDG_SetWindowValue(uint8_t WindowValue);
```

- **范围**：0x40 ~ 0x7F

- **窗口下限**：0x40（固定）

- **窗口上限**：WindowValue（可设置）

#### 3.1.4 设置计数器初值

```c
void WWDG_Enable(uint8_t Counter);
```

- **参数**：Counter（0x40 ~ 0x7F）

- **注意**：此函数同时使能WWDG

#### 3.1.5 喂狗（重载计数器）

```c
void WWDG_SetCounter(uint8_t Counter);
```

**必须在窗口期内调用**（当前计数值 < WindowValue 且 > 0x40）

### 3.2 中断配置

#### 3.2.1 使能早期唤醒中断（EWI）

```c
WWDG_EnableIT();  // 使能中断
WWDG_ClearFlag(); // 清除中断标志

// 配置NVIC
NVIC_InitTypeDef NVIC_InitStruct;
NVIC_InitStruct.NVIC_IRQChannel = WWDG_IRQn;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStruct);
```

#### 3.2.2 中断服务函数

```c
void WWDG_IRQHandler(void) {
    if (WWDG_GetFlagStatus() != RESET) {
        WWDG_ClearFlag();  // 清除中断标志
        // 紧急处理（保存数据等）
        WWDG_SetCounter(0x7F); // 最后喂狗（可选）
    }
}
```

**注意**：此中断在计数器到0x40时触发，是复位前的最后机会

#### 3.2.3 状态监控函数

```c
// 检查是否发生复位
if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET) {
    RCC_ClearFlag();  // 清除标志
}

// 获取当前计数器值
uint8_t current_cnt = WWDG_GetCounter();
```

## 4. 时间参数计算

### 4.1 超时时间公式

```textile
T_wwdg = (4096 × 2^WDGTB × (T[5:0] + 1)) / PCLK1
```

- **WDGTB**：预分频系数（0~3对应分频1/2/4/8）

- **T[5:0]**：计数器低6位值（0~63）

- **PCLK1**：APB1时钟频率（默认36MHz）

### 4.2 窗口时间计算

```textile
T_window = (4096 × 2^WDGTB × (T - WindowValue)) / PCLK1
```

**示例**：WDGTB=3（分频8），T=0x7F=127，WindowValue=0x65=101，PCLK1=36MHz  
窗口时间 = (4096 × 8 × (127 - 101)) / 36Mhz ≈ 23.5ms

1. **喂狗时机**：
   
   - **过早喂狗**（计数器 > WindowValue）→ 复位
   
   - **过晚喂狗**（计数器 < 0x40）→ 复位
   
   - **正确窗口**：WindowValue > 计数器值 > 0x40

2. **初始值要求**：
   
   - 必须 > 0x40 且 > WindowValue
   
   - 推荐设为最大值0x7F

3. **中断使用**：
   
   - EWI中断是复位前最后机会
   
   - 中断中**不能喂狗**（此时已在窗口内）
   
   - 仅用于紧急数据保存


