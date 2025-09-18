# 第十三章 DAC介绍及应用

## 1. DAC简介

DAC 为数字/模拟转换模块，故名思议，它的作用就是把输入的数字编码，转换成对应的模拟电压输出，它的功能与 ADC 相反。在常见的数字信号系统中，大部分传感器信号被化成电压信号，而 ADC 把电压模拟信号转换成易于计算机存储、处理的数字编码，由计算机处理完成后，再由DAC 输出电压模拟信号，该电压模拟信号常常用来驱动某些执行器件，使人类易于感知。如音频信号的采集及还原就是这样一个过程。

STM32 具有片上 DAC 外设，它的分辨率可配置为 8 位或 12 位的数字输入信号，具有两个 DAC输出通道，这两个通道互不影响，每个通道都可以使用 DMA 功能，都具有出错检测能力，可外部触发。

## 2. DAC使用示例

STM32 的 DAC 外设有固定的输出通道，分别为 PA4 和 PA5

要输出正弦波，实质是要控制 DAC 以 v=sin(t) 的正弦函数关系输出电压，其中 v 为电压输出， t为时间。

而由于模拟信号连续而数字信号是离散的，所以使用 DAC 产生正弦波时，只能按一定时间间隔输出正弦曲线上的点，在该时间段内输出相同的电压值，若缩短时间间隔，提高单个周期内的输出点数，可以得到逼近连续正弦波的图形

### 2.1 DAC GPIO初始化

```c
static void DAC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitStructure; 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_Cmd(DAC_Channel_2, ENABLE);
    DAC_DMACmd(DAC_Channel_2, ENABLE);
}

```

### 2.2 DAC TIM初始化

```c
static void DAC_TIM_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = (20-1);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
    TIM_Cmd(TIM2, ENABLE);
}
```

### 2.3 DAC DMA初始化

```c
static void DAC_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (DAC_BASE+0x20);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Double12Bit;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = DAC_Point_Num;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel4, ENABLE);
}

```

### 2.4 DAC Mode初始化

```c
void DAC_Mode_Init(void)
{
    uint32_t Idx = 0;
    DAC_GPIO_Init();
    DAC_TIM_Init();
    for(Idx = 0; Idx < DAC_Point_Num; Idx++)
    {
        Double12Bit[Idx] = (Single12Bit[Idx] << 16) + (Single12Bit[Idx]);
    }
    DAC_DMA_Init();
}

```

### 2.5 DAC主函数测试

```c
#include "stm32f10x.h"
#include "dac.h"
int main(void)
{
    DAC_Mode_Init();
    while(1);
}

```

## 3. 常见DAC函数（STD库）

### 3.1 核心配置函数

#### 3.1.1 初始化与使能

```c
// 初始化DAC通道
void DAC_Init(uint32_t DAC_Channel, DAC_InitTypeDef* DAC_InitStruct);

// 使能/禁用DAC通道
void DAC_Cmd(uint32_t DAC_Channel, FunctionalState NewState);

// 双通道同步初始化（仅大容量型号）
void DAC_DualInit(DAC_InitTypeDef* DAC_InitStruct);
```

#### 3.1.2 参数结构体

```c
typedef struct {
  uint32_t DAC_Trigger;       // 触发源 (DAC_Trigger_None, DAC_Trigger_T2_TRGO等)
  uint32_t DAC_WaveGeneration; // 波形模式 (DAC_WaveGeneration_None/Noise/Triangle)
  uint32_t DAC_LFSRUnmask_TriangleAmplitude; // 波形幅度 (DAC_TriangleAmplitude_1~4095)
  uint32_t DAC_OutputBuffer;   // 输出缓冲 (DAC_OutputBuffer_Enable/Disable)
} DAC_InitTypeDef;
```

#### 3.1.3 数据写入函数

```c
// 设置单通道数据（对齐方式：12位右/左对齐，8位右对齐）
void DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data);  // 通道1
void DAC_SetChannel2Data(uint32_t DAC_Align, uint16_t Data);  // 通道2

// 设置双通道数据（同步更新）
void DAC_SetDualChannelData(uint32_t DAC_Align, uint16_t Data2, uint16_t Data1);

// 读取当前输出值
uint16_t DAC_GetDataOutputValue(uint32_t DAC_Channel);
```

### 3.2 DMA 与触发控制

#### 3.2.1 DMA 配置

```c
// 使能DAC通道的DMA请求
void DAC_DMACmd(uint32_t DAC_Channel, FunctionalState NewState);

// 示例：启动DAC1 DMA传输
DAC_DMACmd(DAC_Channel_1, ENABLE);
```

#### 3.2.2 触发控制

```c
// 软件触发转换
void DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState);

// 配置定时器触发（需在DAC_Init中设置触发源）
// 例如：TIM2触发DAC
TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);  // TIM2更新事件触发
DAC_InitStruct.DAC_Trigger = DAC_Trigger_T2_TRGO;     // DAC响应TIM2触发
```

### 3.3 波形发生器函数

#### 3.3.1 三角波生成

```c
DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1023; // 峰值1023/4095
DAC_Init(DAC_Channel_1, &DAC_InitStruct);
```

#### 3.3.2 噪声波生成

```c
DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_Noise;
DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits10_0; // 10位LFSR掩码
DAC_Init(DAC_Channel_2, &DAC_InitStruct);
```

### 3.4 状态与中断管理

#### 3.4.1 中断控制

```c
// 使能DAC中断（DMA下溢、转换完成等）
void DAC_ITConfig(uint32_t DAC_Channel, uint32_t DAC_IT, FunctionalState NewState);

// 检查中断标志
ITStatus DAC_GetITStatus(uint32_t DAC_Channel, uint32_t DAC_IT);

// 清除中断标志
void DAC_ClearITPendingBit(uint32_t DAC_Channel, uint32_t DAC_IT);
```

#### 3.4.2 标志位检查

```c
// 检查DMA下溢标志
if (DAC_GetFlagStatus(DAC_Channel_1, DAC_FLAG_DMAUDR1)) {
    DAC_ClearFlag(DAC_Channel_1, DAC_FLAG_DMAUDR1); // 清除标志
}
```

### 3.5 双DAC同步模式

```c
// 配置双通道同步输出
DAC_DualInit(&DAC_InitStruct);  // 初始化双通道
DAC_SetDualChannelData(DAC_Align_12b_R, 2048, 1024); // 同步写入两通道数据

// 使用DMA同步输出正弦波（32位数据：高16位=通道2，低16位=通道1）
uint32_t DualSineData[32] = {(sine_ch2[0]<<16) | sine_ch1[0], ...};
DMA_InitStruct.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address; // 双通道寄存器地址
```
