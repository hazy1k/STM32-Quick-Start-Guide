# 第三十三章 高级定时器-PWM输入捕获实验

## 1. 导入

普通的输入捕获可以使用定时器的四个通道， 一路捕获占用一个捕获寄存器，而PWM输入则只能使用两个通道，即通道1和通道2，且一路PWM输入要占用两个捕获寄存器，一个用于捕获周期， 一个用于捕获占空比。在本节实验中，我们用通用定时器产生一路PWM信号，然后用高级定时器的通道1或者通道2来捕获。

## 2. 硬件设计

实验中用到两个引脚，一个是通用定时器TIM3的通道1，即PA6，用于输出PWM信号，另一个是高级控制定时器TIM1的通道1， 即PA8，用于PWM输入捕获，实验中直接用一根杜邦线短接即可PA6和PA8即可，同时可用示波器监控PA6的波形，看看实验捕获的数据是否正确。

## 3. 软件设计

### 3.1 编程大纲

1. 通用定时器产生PWM配置

2. 高级定时器PWM输入配置

3. 编写中断服务程序，计算测量的频率和占空比，并打印出来比较

编程的要点主要分成两部分，一个是通用定时器的PWM信号输出，另一个是PWM信号输入捕获。

### 3.2 代码分析

#### 3.2.1 通用定时器宏定义

```c
#ifndef __GENERALTIM_H
#define __GENERALTIM_H

#include "stm32f10x.h"

/* TIM基础配置 */
#define GTIMx TIM3
#define GTIM_APBxClock RCC_APB1PeriphClockCmd
#define GTIM_CLK RCC_APB1Periph_TIM3
#define GTIM_Period 9
#define GTIM_Prescaler 71

/* 四路输出通道配置 */
#define GTIM_CH1_GPIO_CLK RCC_APB2Periph_GPIOA
#define GTIM_CH1_GPIO_PORT GPIOA
#define GTIM_CH1_GPIO_PIN GPIO_Pin_6
#define GTIM_CH2_GPIO_CLK RCC_APB2Periph_GPIOA
#define GTIM_CH2_GPIO_PORT GPIOA
#define GTIM_CH2_GPIO_PIN GPIO_Pin_7
#define GTIM_CH3_GPIO_CLK RCC_APB2Periph_GPIOB
#define GTIM_CH3_GPIO_PORT GPIOB
#define GTIM_CH3_GPIO_PIN GPIO_Pin_0
#define GTIM_CH4_GPIO_CLK RCC_APB2Periph_GPIOB
#define GTIM_CH4_GPIO_PORT GPIOB
#define GTIM_CH4_GPIO_PIN GPIO_Pin_1

/* CCRx值 */
#define GTIM_CCR1 5
#define GTIM_CCR2 4
#define GTIM_CCR3 3
#define GTIM_CCR4 2

void GTIM_Init(void);

#endif /* __GENERALTIM_H */

```

使用宏定义非常方便程序升级、移植。通过上面的宏，我们可以算出PWM信号的频率F为：72M/( 10*72 )=100KHZ， 占空比为50%。

#### 3.2.2 通用定时器PWM输出

```c
#include "GeneralTim.h"

static void GTIM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// TIM1 CH1 GPIO Configuration
	RCC_APB2PeriphClockCmd(GTIM_CH1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GTIM_CH1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GTIM_CH1_GPIO_PORT, &GPIO_InitStructure);
	// TIM1 CH2 GPIO Configuration
	RCC_APB2PeriphClockCmd(GTIM_CH2_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GTIM_CH2_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GTIM_CH2_GPIO_PORT, &GPIO_InitStructure);
	// TIM1 CH3 GPIO Configuration
	RCC_APB2PeriphClockCmd(GTIM_CH3_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GTIM_CH3_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GTIM_CH3_GPIO_PORT, &GPIO_InitStructure);
	// TIM1 CH4 GPIO Configuration
	RCC_APB2PeriphClockCmd(GTIM_CH4_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GTIM_CH4_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GTIM_CH4_GPIO_PORT, &GPIO_InitStructure);
}

static void GTIM_Mode_Init(void)
{
	GTIM_APBxClock(GTIM_CLK, ENABLE);
	// 时基结构初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = GTIM_Period; // 9
	TIM_TimeBaseStructure.TIM_Prescaler = GTIM_Prescaler; // 71
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(GTIMx, &TIM_TimeBaseStructure);
    // 输出比较模式初始化
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    // CH1
	TIM_OCInitStructure.TIM_Pulse = GTIM_CCR1; // 5
	TIM_OC1Init(GTIMx, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(GTIMx, TIM_OCPreload_Enable); // 预装载使能
	// CH2
	TIM_OCInitStructure.TIM_Pulse = GTIM_CCR2; // 4
	TIM_OC2Init(GTIMx, &TIM_OCInitStructure);
	// CH3
	TIM_OCInitStructure.TIM_Pulse = GTIM_CCR3; // 3
	TIM_OC3Init(GTIMx, &TIM_OCInitStructure);
	// CH4
	TIM_OCInitStructure.TIM_Pulse = GTIM_CCR4; // 2
	TIM_OC4Init(GTIMx, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(GTIMx, TIM_OCPreload_Enable);
	TIM_Cmd(GTIMx, ENABLE);
}

void GTIM_Init(void)
{
	GTIM_GPIO_Init();
	GTIM_Mode_Init();
}

```

#### 3.2.3 高级定时器宏定义

```c
#ifndef __ADVANCE_TIM_H
#define __ADVANCE_TIM_H

#include "stm32f10x.h"

/* TIM1基础配置 */
#define ATIMx TIM1
#define ATIM_APBxClock RCC_APB2PeriphClockCmd
#define ATIM_CLK RCC_APB2Periph_TIM1
#define ATIM_Period 999
#define ATIM_Prescaler 71
/* 输入捕获通道配置 */
#define ATIM_CH1_GPIO_CLK RCC_APB2Periph_GPIOA
#define ATIM_CH1_GPIO_PORT GPIOA
#define ATIM_CH1_GPIO_PIN GPIO_Pin_8
#define ATIM_IC1PWM_CHANNEL TIM_Channel_1
#define ATIM_IC2PWM_CHANNEL TIM_Channel_2
/* 中断相关配置 */
#define ATIM_IRQ TIM1_CC_IRQn
#define ATIM_IRQHandler TIM1_CC_IRQHandler

void ATIMx_Init(void);

#endif /* __ADVANCE_TIM_H */

```

在上面的宏定义里面，我们可以算出计数器的计数周期为T = 72M/(1000*72) = 1ms，这个是定时器在不溢出的情况下的最大计数周期， 也就是说周期小于1ms的PWM信号都可以被捕获到，转换成频率就是能捕获到的最小的频率为1KHZ。

#### 3.2.4 高级定时器PWM输入捕获模式

```c
#include "AdvanceTim.h"

static void ATIM_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = ATIM_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void ATIM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(ATIM_CH1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ATIM_CH1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(ATIM_CH1_GPIO_PORT, &GPIO_InitStructure);
}

static void ATIM_Mode_Init(void)
{
	ATIM_APBxClock(ATIM_CLK, ENABLE); // 72 MHz
	// 时基结构初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = ATIM_Period; // 999
	TIM_TimeBaseStructure.TIM_Prescaler = ATIM_Prescaler; // 71
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ATIMx, &TIM_TimeBaseStructure);
	// 输入捕获结构初始化
	// 捕获PWM时需要两个捕获寄存器，一个测周期，一个测占空比
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = ATIM_IC1PWM_CHANNEL;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 捕获上升沿
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0; // 不滤波
	TIM_PWMIConfig(ATIMx, &TIM_ICInitStructure);
	/* 当工作做PWM输入模式时，只需要设置触发信号的那一路即可（用于测量周期
	   另外一路（用来测量占空比）会由硬件自带设置，所以不需要设置。*/
	// 捕获通道IC2配置
	/*TIM_ICInitStructure.TIM_Channel = ATIM_IC1PWM_CHANNEL;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; // 捕获下降沿
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0; 
	TIM_PWMIConfig(ATIMx, &TIM_ICInitStructure);*/
	// 选择输入捕获的触发信号
	TIM_SelectInputTrigger(ATIMx, TIM_TS_TI1FP1); // 选择TI1FP1引脚作为触发信号
	// 选择从模式：复位模式
	// PWM输入模式时，从模式工作在复位模式，当捕获开始时，计数器CNT复位
	TIM_SelectSlaveMode(ATIMx, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(ATIMx, TIM_MasterSlaveMode_Enable); // 使能主从模式
	TIM_ITConfig(ATIMx, TIM_IT_CC1, ENABLE);
	TIM_ClearITPendingBit(ATIMx, TIM_IT_CC1);
	TIM_Cmd(ATIMx, ENABLE);
}

void ATIMx_Init(void)
{
	ATIM_NVIC_Init();
	ATIM_GPIO_Init();
	ATIM_Mode_Init();
}

```

因为是PWM输入模式，只能使用通道1和通道2 ，假如我们使用的是通道1，即TI1，输入的PWM信号会被分成两路，分别是TI1FP1和TI1FP2， 两路都可以是触发信号。如果选择TI1FP1为触发信号，那么IC1捕获到的是PWM信号的周期， IC2捕获到的是占空比， 这种输入通道TI和捕获通道IC的映射关系叫直连，输入捕获结构体的TIM_ICSelection要配置为TIM_ICSelection_DirectTI。如果选择TI1FP2为触发信号， 则IC2捕获到的是周期，IC1捕获到的是占空比，这种输入通道TI和捕获通道IC的映射关系叫非直连， 输入捕获结构体的TIM_ICSelection要配置为TIM_ICSelection_IndirectTI。 有关输入通道TI和捕获通道IC的具体映射关系见图，有直连和非直连两种。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen012.png)

#### 3.2.5 高级定时器中断服务函数

```c
__IO uint16_t IC2Value = 0; // 捕获频率
__IO uint16_t IC1Value = 0; // 捕获占空比
__IO float DutyCycle = 0;   // 计算占空比
__IO float Frequency = 0;   // 计算频率

/*
 * 如果是第一个上升沿中断，计数器会被复位，锁存到CCR1寄存器的值是0，CCR2寄存器的值也是0
 * 无法计算频率和占空比。当第二次上升沿到来的时候，CCR1和CCR2捕获到的才是有效的值。其中
 * CCR1对应的是周期，CCR2对应的是占空比。
 */
void ATIM_IRQHandler(void)
{
  TIM_ClearITPendingBit(ATIMx, TIM_IT_CC1);
  IC1Value = TIM_GetCapture1(ATIMx);
  IC2Value = TIM_GetCapture2(ATIMx);
	printf("IC1Value = %d  IC2Value = %d ",IC1Value,IC2Value); // 输出捕获值
  // 注意：捕获寄存器CCR1和CCR2的值在计算占空比和频率的时候必须加1，因为计数器是从 0 开始计数的。
	if (IC1Value != 0)
  {
    DutyCycle = (float)((IC2Value+1) * 100) / (IC1Value+1); // 计算占空比
    Frequency = (72000000/(ATIM_Prescaler+1))/(float)(IC1Value+1); // 计算频率
		printf("占空比：%0.2f%%   频率：%0.2fHz\n",DutyCycle,Frequency);
  }
  else
  {
    DutyCycle = 0;
    Frequency = 0;
  }
}
```

当捕获到PWM信号的第一个上升沿时，产生中断，计数器被复位，锁存到捕获寄存器IC1和IC2的值都为0。当下降沿到来时， IC2会捕获，对应的是占空比，但是会产生中断。当捕获到第二个下降沿时，IC1会捕获，对应的是周期，而且会再次进入中断， 这个时间就可以根据IC1和IC2的值计算出频率和占空比。有关PWM输入的时序见图

中断复位函数中，我们获取输入捕获寄存器CCR1和CCR2寄存器中的值，当CCR1的值不为0时，说明有效捕获到了一个周期， 然后计算出频率和占空比。在计算的时候CCR1和CCR2的值都必须要加1，因为计数器是从0开始计数的。

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen013.png" title="" alt="" width="1000">

#### 3.2.6 主函数

```c
int main(void)
{
    USART_Config();
    GTIM_Init();
    ATIM_Init();
    while (1) 
    {

    }
}
```

main函数非常简单，通用定时器初始化完之后用于输出PWM信号，高级定时器初始化完之后用于捕获通用定时器输出的PWM信号。

## 4. 小结

下面我们来简单回顾一下流程吧：

### 4.1 实验流程

1. **硬件连接**
   
   - 将通用定时器的PWM输出引脚（例如`TIM2_CH1`）连接到高级定时器的捕获输入引脚（例如`TIM1_CH1`）。
   - 确保STM32F103的时钟设置正确，以便定时器能正常工作。

2. **软件配置**
   
   - 配置系统时钟，以确保定时器工作在预期频率。
   - 配置通用定时器（例如`TIM2`）以生成PWM信号。
   - 配置高级定时器（例如`TIM1`）以捕获PWM信号。
   - 配置相应的GPIO引脚作为定时器的输入和输出。
   - 配置NVIC中断处理程序（如果需要）。

### 4.2 代码示例

以下是一个基于STM32标准外设库的代码示例，展示了如何配置`TIM2`生成PWM信号，并使用`TIM1`捕获该信号。

```c
#include "stm32f10x.h"

// Function prototypes
void TIM2_PWM_Init(void);
void TIM1_IC_Init(void);
void GPIO_Config(void);

int main(void) {
    // Initialize GPIO for PWM output and IC input
    GPIO_Config();

    // Initialize TIM2 for PWM output
    TIM2_PWM_Init();

    // Initialize TIM1 for input capture
    TIM1_IC_Init();

    while (1) {
        // Main loop
    }
}

void GPIO_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable GPIO clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Configure PA0 (TIM2 CH1) as alternate function push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure PA8 (TIM1 CH1) as alternate function push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TIM2_PWM_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // Enable TIM2 clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // Time base configuration
    TIM_TimeBaseStructure.TIM_Period = 999; // Auto-reload register
    TIM_TimeBaseStructure.TIM_Prescaler = 71; // Prescaler value
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // PWM mode configuration
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 499; // Compare value (duty cycle)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    // Enable TIM2
    TIM_Cmd(TIM2, ENABLE);
}

void TIM1_IC_Init(void) {
    TIM_ICInitTypeDef TIM_ICInitStructure;

    // Enable TIM1 clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // Time base configuration (you may adjust this as needed)
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // Input capture configuration
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);

    // Enable TIM1
    TIM_Cmd(TIM1, ENABLE);
}
```

### 4.3 代码说明

1. **`GPIO_Config()`**: 配置`PA0`为`TIM2_CH1`输出，`PA8`为`TIM1_CH1`输入。
2. **`TIM2_PWM_Init()`**: 配置`TIM2`为PWM模式，设置周期和占空比。
3. **`TIM1_IC_Init()`**: 配置`TIM1`为输入捕获模式，捕获`TIM2`输出的PWM信号。

---

2024.9.16 第一次修订，后期不再修订

2025.1.28 修订代码
