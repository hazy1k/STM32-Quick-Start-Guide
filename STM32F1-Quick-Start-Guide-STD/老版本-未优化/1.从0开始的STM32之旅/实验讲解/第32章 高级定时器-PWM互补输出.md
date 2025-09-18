# 第三十二章 高级定时器-PWM互补输出

## 1. 硬件设计

根据开发板引脚使用情况，并且参考定时器引脚信息 ，使用高级定时器TIM1的通道1及其互补通道作为本实验的波形输出通道，对应选择PA8和PB13引脚。将示波器的两个输入通道分别与PA8和PB13引脚连接，用于观察波形，还有注意共地。在指南者开发板里面，PA8通过一个调帽默认连接了蜂鸣器，如果调帽不拔掉的话，PA8输出的PWM信号会让蜂鸣器响。

为增加断路功能，需要用到TIM1_BKIN引脚，这里选择PB12引脚。程序我们设置该引脚为高电平有效，当BKIN引脚被置高低电平的时候，两路互补的PWM输出就被停止，就好像是刹车一样。

## 2. 软件设计

### 2.1 编程大纲

1. TIM相关参数宏定义

2. TIM GPIO初始化

3. 时基结构、输出比较结构、死区结构初始化

### 2.2 代码分析

#### 2.2.1 TIM相关参数宏定义

```c
/* TIMx */
#define ATIMx TIM1
#define ATIM_APBxClock RCC_APB2PeriphClockCmd
#define ATIM_CLK RCC_APB2Periph_TIM1
/* PWN信号 */
// F = TIM_CLK/(ARR+1)*(PSC+1) = 72/8*9 = 1
#define ATIM_Period 7
#define ATIM_Prescaler 8
#define ATIM_Pulse 4
/* 输出比较通道 */
#define ATIM_CH1_GPIO_CLK RCC_APB2Periph_GPIOA // PA8
#define ATIM_CH1_GPIO_PORT GPIOA
#define ATIM_CH1_GPIO_PIN GPIO_Pin_8
/* 输出比通道的互补通道 */
#define ATIM_CH1N_GPIO_CLK RCC_APB2Periph_GPIOB // PB13
#define ATIM_CH1N_GPIO_PORT GPIOB
#define ATIM_CH1N_GPIO_PIN GPIO_Pin_13
/* 输出比较通道的刹车通道 */
#define ATIM_BKIN_GPIO_CLK RCC_APB2Periph_GPIOB // PB12
#define ATIM_BKIN_GPIO_PORT GPIOB
#define ATIM_BKIN_GPIO_PIN GPIO_Pin_12
```

#### 2.2.2 定时器复用功能引脚初始化

```c
static void ATIM_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // 输出比较通道GPIO初始化
    RCC_APB2PeriphClockCmd(ATIM_CH1_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = ATIM_CH1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ATIM_CH1_GPIO_PORT, &GPIO_InitStructure);
    // 输出比较通道的互补通道GPIO初始化
    RCC_APB2PeriphClockCmd(ATIM_CH1N_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = ATIM_CH1N_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ATIM_CH1N_GPIO_PORT, &GPIO_InitStructure);
    // 输出比较通道的刹车通道GPIO初始化
    RCC_APB2PeriphClockCmd(ATIM_BKIN_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = ATIM_BKIN_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ATIM_BKIN_GPIO_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(ATIM_BKIN_GPIO_PORT, ATIM_BKIN_GPIO_PIN); // BKIN默认是低电平状态  
}
```

#### 2.2.3 定时器模式配置

```c
static void ATIM_Mode_Init(void)
{
    ATIM_APBxClock(ATIM_CLK, ENABLE); // 72MHz
    // 时基结构初始化
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = ATIM_Period; // 8-1
    TIM_TimeBaseStructure.TIM_Prescaler = ATIM_Prescaler; // 9-1
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(ATIMx, &TIM_TimeBaseStructure);
    // 输出比较结构初始化
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // 配置为PWM1模式
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 输出使能
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 互补输出使能
    TIM_OCInitStructure.TIM_Pulse = ATIM_Pulse; // 占空比4 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; // 互补通道输出极性高
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set; // 输出通道空闲状态置位
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset; // 互补通道空闲状态复位
    TIM_OC1Init(ATIMx, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(ATIMx, TIM_OCPreload_Enable); // 预装载使能
    // 死区结构初始化
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable; // 允许重装载
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable; // 允许停止
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; // 锁定
    TIM_BDTRInitStructure.TIM_DeadTime = 11; // 死区时间11
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable; // 允许死区
    // 当BKIN引脚检测到高电平的时候，输出比较信号立即被禁止，直到下一个更新事件发生。
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High; // 死区极性高
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable; // 允许自动输出
    TIM_BDTRConfig(ATIMx, &TIM_BDTRInitStructure);
    TIM_Cmd(ATIMx, ENABLE);
    TIM_CtrlPWMOutputs(ATIMx, ENABLE); // 使能PWM输出
}
```

ADVANCE_TIM_Mode_Config()函数中初始化了三个结构体，有关这三个结构体成员的具体含义可参考“定时器初始化结构体详解”小节， 剩下的程序参考注释阅读即可。如果需要修改PWM的周期和占空比，修改头文件里面的ADVANCE_TIM_PERIOD、 ADVANCE_TIM_PSC和ADVANCE_TIM_PULSE这三个宏即可。 PWM信号的频率的计算公司为：F =TIM_CLK/{(ARR+1)*(PSC+1)}， 其中TIM_CLK等于72MHZ，ARR即自动重装载寄存器的值，对应ADVANCE_TIM_PERIOD这个宏，PSC即计数器时钟的分频因子，对应ADVANCE_TIM_PSC这个宏。

#### 2.2.4 主函数

```c
// TIM—高级定时器-PWM互补输出带死区时间应用
#include "stm32f10x.h"
#include "led.h"
#include "AdvanceTim.h"  

int main(void)
{
  ATIMx_Config();
  while(1)
  {      
  }
}
```

Main函数很简单，调用了ADVANCE_TIM_Init()函数，该函数调用了ADVANCE_TIM_GPIO_Config()和ADVANCE_TIM_Mode_Config()这两个函数完成了定时器GPIO引脚和工作模式的初始化， 这时，相应的GPIO引脚上就可以检测到互补输出的PWM信号，而且带死区时间，如果程序运行的过程中， BKIN引脚被拉高的话，PWM输出会被禁止，就好像是断路或者刹车一样。

## 3. 小结

看似比较复杂，其实就是一个定时器配置的问题，相比与前面这一章多了一个死区时间的配置而已，下面我们总结一下流程：

1. **定时器时钟配置**：

```c
ADVANCE_TIM_APBxClock_FUN(ADVANCE_TIM_CLK, ENABLE);
```

这行代码用于开启定时器的时钟。具体的时钟源和配置会依赖于你的硬件和定时器的设置。

2. **时基结构体初始化**：

```c
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_TimeBaseStructure.TIM_Period = ADVANCE_TIM_PERIOD;
TIM_TimeBaseStructure.TIM_Prescaler = ADVANCE_TIM_PSC;
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);
```

- `TIM_Period`：定时器的自动重装载寄存器的值，即定时器计数器溢出的周期。
- `TIM_Prescaler`：设置计数器的分频系数，影响计数器的时钟频率。
- `TIM_ClockDivision`：配置时钟的分频因子，通常用于配置死区时间等。
- `TIM_CounterMode`：设置计数模式，这里是向上计数模式。
- `TIM_RepetitionCounter`：用于设置重复计数器的值，通常不使用时设为0。
3. **输出比较结构体初始化**：

```c
TIM_OCInitTypeDef TIM_OCInitStructure;
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
TIM_OCInitStructure.TIM_Pulse = ADVANCE_TIM_PULSE;
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
TIM_OC1Init(ADVANCE_TIM, &TIM_OCInitStructure);
TIM_OC1PreloadConfig(ADVANCE_TIM, TIM_OCPreload_Enable);
```

- `TIM_OCMode`：配置为PWM模式1，用于产生PWM信号。
- `TIM_OutputState`：使能输出比较功能。
- `TIM_OutputNState`：使能互补输出功能。
- `TIM_Pulse`：设置PWM信号的占空比。
- `TIM_OCPolarity`：设置PWM信号的极性。
- `TIM_OCNPolarity`：设置互补PWM信号的极性。
- `TIM_OCIdleState`：设置PWM信号在空闲状态下的电平。
- `TIM_OCNIdleState`：设置互补PWM信号在空闲状态下的电平。
- `TIM_OC1PreloadConfig`：使能预装载寄存器，用于在下一个计数周期装载新的比较值。

**刹车和死区结构体初始化**：

```c
TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
TIM_BDTRInitStructure.TIM_DeadTime = 11;
TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
TIM_BDTRConfig(ADVANCE_TIM, &TIM_BDTRInitStructure);
```

- `TIM_OSSRState` 和 `TIM_OSSIState`：配置定时器的刹车功能。
- `TIM_LOCKLevel`：配置定时器锁定级别，防止修改配置寄存器。
- `TIM_DeadTime`：设置死区时间，这里设置为11个计数周期。
- `TIM_Break`：使能刹车功能，当BKIN引脚检测到高电平时，输出信号被禁止。
- `TIM_BreakPolarity`：设置刹车信号的极性。
- `TIM_AutomaticOutput`：使能自动输出功能。

**使能计数器和主输出**：

```c
TIM_Cmd(ADVANCE_TIM, ENABLE);
TIM_CtrlPWMOutputs(ADVANCE_TIM, ENABLE);
```

- `TIM_Cmd`：使能定时器计数器。
- `TIM_CtrlPWMOutputs`：使能PWM输出，通常在使用高级定时器时需要启用。

---

### 3.1**基本原理**

1. **PWM信号生成**：
   
   - PWM（脉宽调制）信号是通过调节信号的高电平持续时间（脉宽）来控制功率。定时器产生PWM信号通过设置比较寄存器的值和计数器的周期来实现。

2. **刹车功能**：
   
   - 刹车功能在定时器的刹车事件发生时关闭输出信号，保护电路或负载。当刹车信号被激活时，定时器的输出被迫置为低电平。

3. **死区时间**：
   
   - 死区时间用于避免PWM信号的两个互补输出同时导通，防止短路。它在两个互补信号之间引入一个小的时间间隔。

### **3.2 详细代码示例**

以下是一个高级定时器的配置示例（以STM32系列为例）：

```c
#include "stm32f4xx.h"

void TIM_Config(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
    // 1. 使能定时器时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM1, ENABLE);
    // 2. 定时器时基配置
    TIM_TimeBaseStructure.TIM_Period = 999;          // 自动重装载寄存器周期
    TIM_TimeBaseStructure.TIM_Prescaler = 83;         // 预分频器
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    // 3. 输出比较配置
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 499;              // 占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    // 4. 刹车和死区时间配置
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
    TIM_BDTRInitStructure.TIM_DeadTime = 11;          // 死区时间
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
    // 5. 使能定时器
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}
```

### **3.2 应用**

- **电机控制**：在电机驱动中，PWM信号用于控制电机的速度和转向，刹车功能保护电机免受过流损坏。
- **风扇控制**：通过PWM信号调节风扇的转速。
- **LED调光**：使用PWM调节LED的亮度。

---

2024.9.14 第一次修订，后期不再维护

2025.1.26 修订代码
