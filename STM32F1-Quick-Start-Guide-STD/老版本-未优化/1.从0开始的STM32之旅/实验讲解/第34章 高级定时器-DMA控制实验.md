# 第三十四章 高级定时器-DMA控制实验

## 1. 硬件设计

TIM和DMA均输入内部资源，无需硬件设计，还使用了按键，已经分析过了

## 2. 软件设计

### 2.1 编程大纲

1. TIM和DMA相关参数宏定义

2. TIM GPIO及DMA Mode配置

3. TIM Mode配置

4. 主函数测

### 2.2 代码分析

#### 2.2.1 相关参数宏定义

```c
#ifndef __ATIM_H
#define __ATIM_H

#include "stm32f10x.h"

/* TIM1输出比较通道1 */
#define ATIMx TIM1
#define ATIM_CLK RCC_APB2Periph_TIM1
#define ATIM_CH1_GPIO_CLK RCC_APB2Periph_GPIOA
#define ATIM_CH1_GPIO_PORT GPIOA
#define ATIM_CH1_GPIO_PIN GPIO_Pin_8

/* DMA配置*/
#define ATIM_DMA_ADDRESS TIM1_BASE+0x34
#define ATIM_DMA_BufferSize 3
#define ATIM_DMA_CLK RCC_AHBPeriph_DMA1
#define ATIM_DMA_Channel DMA1_Channel2

void ATIMx_Init(void);

#endif /* __ATIM_H */

```

#### 2.2.2 高级定时器初始化，DMA初始化

```c
static void ATIM_GPIO_Init(void)
{
  // CH1 GPIO Configuration
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(ATIM_CH1_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = ATIM_CH1_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(ATIM_CH1_GPIO_PORT, &GPIO_InitStructure);
  // DMA Configuration
  DMA_InitTypeDef DMA_InitStructure;
  RCC_AHBPeriphClockCmd(ATIM_DMA_CLK, ENABLE);
  DMA_DeInit(ATIM_DMA_Channel);
  // DMA配置
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(ATIM_DMA_ADDRESS); // 设置DMA源地址：TIM的CCR寄存器
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ATIM_Buffer; // 设置DMA目的地址：ATIM_Buffer
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; // 设置DMA传输方向：从内存到外设
  DMA_InitStructure.DMA_BufferSize = ATIM_DMA_BufferSize; // 设置DMA传输数据长度
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(ATIM_DMA_Channel, &DMA_InitStructure);
}

```



```c
 // DMA初始化
 // DMA时钟使能
  RCC_AHBPeriphClockCmd(TIM_DMA_CLK, ENABLE);    
  DMA_DeInit(TIM_DMA_STREAM);

  // 设置DMA源地址：TIM的CCR寄存器地址
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(TIM_DMA_ADDRESS) ;
  // 内存地址(要传输的变量的指针)
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)aSRC_Buffer;
  // 方向：从内存到外设    
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  // 传输数目    
  DMA_InitStructure.DMA_BufferSize = TIM_DMA_BUFSIZE; // 3
  // 外设地址不增    
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  // 内存地址自增
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  // 外设数据单位    
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  // 内存数据单位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
  // DMA模式，一次或者循环模式
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  // 优先级：中
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  // 禁止内存到内存的传输
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable ;
  // 配置DMA通道    
  DMA_Init(TIM_DMA_STREAM, &DMA_InitStructure);
}
```

#### 2.2.3 高级定时器模式配置

```c
static void ATIM_Mode_Init(void)
{
  // 时基结构初始化
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB2PeriphClockCmd(ATIM_CLK, ENABLE);
  TIM_TimeBaseStructure.TIM_Period = 1000-1;
  TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(ATIMx, &TIM_TimeBaseStructure);
  // 输出PWM模式配置
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(ATIMx, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(ATIMx, TIM_OCPreload_Enable);
  TIM_Cmd(ATIMx, ENABLE);
  // DMA使能
  DMA_Cmd(ATIM_DMA_Channel, ENABLE);
  TIM_DMACmd(ATIMx, TIM_DMA_CC1, ENABLE);
  TIM_CtrlPWMOutputs(ATIMx, ENABLE); // 使能PWM输出
}
```

#### 2.2.4 主函数

```c
#include "stm32f10x.h"
#include "atim.h"
#include "key.h" 

extern uint16_t ATIM_Buffer[3];

int main(void)
{
  KEY_GPIO_Init();
  ATIMx_Init();
  while(1)
  {
    if(KEY_Scan(KEY1_GPIO, KEY1_GPIO_PIN) == KEY_ON)
    {
      ATIM_Buffer[0] = 800;
      ATIM_Buffer[1] = 800;
      ATIM_Buffer[2] = 100;
    }
    if(KEY_Scan(KEY2_GPIO, KEY2_GPIO_PIN) == KEY_ON)
    {
      ATIM_Buffer[0] = 300;
      ATIM_Buffer[1] = 300;
      ATIM_Buffer[2] = 100;
    }
  }
}

```

检查按键按下后，修改数组的值，从而改变PWM波形

## 3. 小结

### 3.1 实验目标

1. 配置高级定时器生成PWM信号。
2. 使用DMA来自动更新PWM的占空比。
3. 按下不同的按键来改变PWM信号的占空比。

### 3.2 硬件要求

1. STM32微控制器（例如STM32F4系列）。
2. 两个按键，连接到GPIO端口。
3. 一个LED或示波器，连接到PWM输出引脚以观察PWM信号。

### 3.3 实验步骤

#### 1. 系统时钟配置

确保系统时钟配置正确，以提供足够的时钟给定时器和DMA。

#### 2. GPIO配置

配置用于PWM输出的GPIO引脚及按键引脚。

#### 3. 定时器配置

配置高级定时器以生成PWM信号。

#### 4. DMA配置

配置DMA以在定时器更新事件时自动更新PWM的占空比。

#### 5. 按键扫描

配置GPIO扫描按键状态，并根据按键状态改变PWM波形。

#### 6. 主程序

实现主程序逻辑，管理按键输入和PWM波形更新。

### 3.4 代码示例

以下代码示例基于STM32F4系列微控制器，使用HAL库（硬件抽象层）来配置硬件资源。根据你的具体微控制器型号和开发环境，你可能需要调整代码。

```c
#include "stm32f4xx_hal.h"

// 定义按键状态
#define KEY1_GPIO_PORT GPIOA
#define KEY1_GPIO_PIN GPIO_PIN_0
#define KEY2_GPIO_PORT GPIOA
#define KEY2_GPIO_PIN GPIO_PIN_1

// 定义PWM占空比数组
uint16_t aSRC_Buffer[3] = {1000, 1000, 100};

// 函数声明
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM1_Init(void);
static void MX_NVIC_Init(void);

// 定时器和DMA句柄
TIM_HandleTypeDef htim1;
DMA_HandleTypeDef hdma_tim1_up;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_TIM1_Init();
    MX_NVIC_Init();

    // 启动定时器和DMA
    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t*)aSRC_Buffer, 3);

    while (1)
    {
        if (HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == GPIO_PIN_SET)
        {
            // 按下按键1
            aSRC_Buffer[0] = 1000;
            aSRC_Buffer[1] = 1000;
            aSRC_Buffer[2] = 100;
        }
        else if (HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == GPIO_PIN_SET)
        {
            // 按下按键2
            aSRC_Buffer[0] = 300;
            aSRC_Buffer[1] = 300;
            aSRC_Buffer[2] = 100;
        }

        HAL_Delay(100);  // 延时去抖动和减少CPU占用
    }
}

// 定时器1初始化函数
static void MX_TIM1_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 8399;  // 根据时钟频率调整
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 9999;     // 根据PWM频率调整
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&htim1);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;  // 初始占空比
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
}

// DMA初始化函数
static void MX_DMA_Init(void)
{
    __HAL_RCC_DMA2_CLK_ENABLE();

    hdma_tim1_up.Instance = DMA2_Stream5;
    hdma_tim1_up.Init.Channel = DMA_CHANNEL_6;
    hdma_tim1_up.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim1_up.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim1_up.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim1_up.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim1_up.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_tim1_up.Init.Mode = DMA_CIRCULAR;
    hdma_tim1_up.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim1_up.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_tim1_up);

    __HAL_LINKDMA(&htim1, hdma[TIM_DMA_ID_UPDATE], hdma_tim1_up);
}

// GPIO初始化函数
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;  // 配置按键引脚
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置PWM输出引脚
    GPIO_InitStruct.Pin = GPIO_PIN_8;  // 根据你的引脚配置
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// NVIC初始化函数
static void MX_NVIC_Init(void)
{
    HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
}

// 时钟配置函数
void SystemClock_Config(void)
{
    // 根据你的系统时钟设置代码
}

// DMA中断处理函数
void DMA2_Stream5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_tim1_up);
}
```

### 3.5 代码解释

1. **`MX_TIM1_Init`**: 初始化定时器1为PWM模式，设置PWM周期和初始占空比。

2. **`MX_DMA_Init`**: 初始化DMA，设置内存到外设传输模式，配置为循环模式。

3. **`MX_GPIO_Init`**: 配置GPIO引脚用于按键输入和PWM输出。

4. **`main`函数**: 启动PWM和DMA，监控按键状态，并根据按键输入改变PWM占空比。每次修改 `aSRC_Buffer` 后，DMA会自动更新PWM信号。

5. **`SystemClock_Config`**: 配置系统时钟，根据你的实际硬件和时钟频率进行设置。

---

2024.9.14 第一次修订，后期不再维护

2025.1.28 修订代码
