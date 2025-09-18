# 第十二章 ADC介绍及应用

## 1. ADC简介

STM32f103 系列有 3 个 ADC，精度为 12 位，每个 ADC 最多有 16 个外部通道。其中 ADC1 和ADC2 都有 16 个外部通道， ADC3 根据 CPU 引脚的不同通道数也不同，一般都有 8 个外部通道。

我们确定好 ADC 输入电压之后，那么电压怎么输入到 ADC？这里我们引入通道的概念， STM32 的ADC 多达 18 个通道，其中外部的 16 个通道就是框图中的 ADCx_IN0、 ADCx_IN1… ADCx_IN5。这 16 个通道对应着不同的 IO 口，具体是哪一个 IO 口可以从手册查询到。其中 ADC1/2/3 还有内部通道： ADC1 的通道 16 连接到了芯片内部的温度传感器， Vrefint 连接到了通道 17。 ADC2 的模拟通道 16 和 17 连接到了内部的 VSS。 ADC3 的模拟通道 9、 14、 15、 16 和 17 连接到了内部的 VSS。

![屏幕截图 2025-06-30 220530.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/06/30-22-05-36-屏幕截图%202025-06-30%20220530.png)

## 2. ADC初始化结构体

标准库函数对每个外设都建立了一个初始化结构体 xxx_InitTypeDef(xxx 为外设名称)，结构体成员用于设置外设工作参数，并由标准库函数 xxx_Init() 调用这些设定参数进入设置外设相应的寄存器，达到配置外设工作环境的目的

```c
typedef struct
{    
    uint32_t ADC_Mode; // ADC 工作模式选择
    FunctionalState ADC_ScanConvMode; /* ADC 扫描（多通道）
                    或者单次（单通道）模式选择 */
    FunctionalState ADC_ContinuousConvMode; // ADC 单次转换或者连续转换选择
    uint32_t ADC_ExternalTrigConv; // ADC 转换触发信号选择
    uint32_t ADC_DataAlign; // ADC 数据寄存器对齐格式
    uint8_t ADC_NbrOfChannel; // ADC 采集通道数
} ADC_InitTypeDef;
```

1. **`ADC_Mode`**:
   
   - **功能**：设置 ADC 的工作模式。
   - **可能值**：通常包括单通道模式、双通道模式、连续转换模式等。具体值由硬件架构或库定义。

2. **`ADC_ScanConvMode`**:
   
   - **功能**：选择是否启用扫描模式。
   - **可能值**：
     - `ENABLE`：启用扫描模式，适用于多个通道的连续采样。
     - `DISABLE`：禁用扫描模式，适用于单通道采样。
   - **扫描模式**表示 ADC 会按顺序对多个通道进行采样，而 **单次模式** 则只对单一通道进行采样。

3. **`ADC_ContinuousConvMode`**:
   
   - **功能**：选择 ADC 的转换模式，可以选择单次转换或连续转换。
   - **可能值**：
     - `ENABLE`：启用连续转换模式，即一旦转换完成，ADC 会自动启动下一次转换。
     - `DISABLE`：禁用连续转换模式，ADC 完成一次转换后停止。

4. **`ADC_ExternalTrigConv`**:
   
   - **功能**：设置 ADC 转换触发信号的来源。
   - **可能值**：此值由具体的硬件平台定义，可以选择外部触发信号，如定时器输出、外部引脚等。

5. **`ADC_DataAlign`**:
   
   - **功能**：设置 ADC 数据寄存器的对齐方式。
   - **可能值**：
     - `ADC_DATAALIGN_RIGHT`：数据右对齐，通常用于较低位的数据。
     - `ADC_DATAALIGN_LEFT`：数据左对齐，通常用于较高位的数据。

6. **`ADC_NbrOfChannel`**:
   
   - **功能**：设置要采集的 ADC 通道的数量。
   - **可能值**：该值通常为 1 至多个通道，具体通道数取决于所选的工作模式和硬件平台。

## 3. ADC使用示例

### 3.1 独立模式单通道采集

STM32 的 ADC 功能繁多，我们设计三个实验尽量完整的展示 ADC 的功能。首先是比较基础实用的单通道采集，实现开发板上电位器的动触点输出引脚电压的采集并通过串口打印至 PC 端串口调试助手。单通道采集适用 AD 转换完成中断，在中断服务函数中读取数据，不使用 DMA 传输，在多通道采集时才使用 DMA 传输。

开发板板载一个贴片滑动变阻器:

![屏幕截图 2025-06-30 224358.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/06/30-22-44-01-屏幕截图%202025-06-30%20224358.png)

贴片滑动变阻器的动触点通过连接至 STM32 芯片的 ADC 通道引脚。当我们使用旋转滑动变阻器调节旋钮时，其动触点电压也会随之改变，电压变化范围为 0~3.3V，亦是开发板默认的 ADC电压采集范围。

#### 3.1.1 ADC相关参数宏定义

```c
#ifndef __ADC_H__
#define __ADC_H__ 

#include "stm32f10x.h"
#include "led.h"

#define ADCx ADC2
#define ADCx_Channel ADC_Channel_11
#define ADC_GPIO_PORT GPIOC
#define ADC_GPIO_PIN GPIO_Pin_1
#define ADC_IRQn ADC1_2_IRQn
#define ADC_IRQHandler ADC1_2_IRQHandler

void ADC_Mode_Init(void);

#endif /* __ADC_H__ */
```

#### 3.1.2 ADC配置

```c
#include "adc.h"

__IO uint16_t ADC_ConverValue; // ADC转换值

static void ADC_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void ADC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入
    GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
}

void ADC_Mode_Init(void)
{
    ADC_GPIO_Init();
    ADC_NVIC_Init();
    ADC_InitTypeDef ADC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    /* ADC Mode 配置 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // 独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; // 禁止扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // 连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 无外部触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 
    ADC_InitStructure.ADC_NbrOfChannel = 1; // 1个通道
    ADC_Init(ADCx, &ADC_InitStructure);
    /* 时钟配置 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8); // 72/8 = 9MHz
    ADC_RegularChannelConfig(ADCx, ADCx_Channel, 1, ADC_SampleTime_55Cycles5); // 配置ADC通道，采样时间为55.5个时钟周期
    /* 中断配置与初始化 */
    ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE); // 转换结果产生时中断
    ADC_Cmd(ADCx, ENABLE);
    ADC_ResetCalibration(ADCx); // 重置校准
    while (ADC_GetResetCalibrationStatus(ADCx)); // 等待重置完成
    ADC_StartCalibration(ADCx); // 开始校准
    while (ADC_GetCalibrationStatus(ADCx)); // 等待校准完成
    ADC_SoftwareStartConvCmd(ADCx, ENABLE); // 开始转换   
}
```

#### 3.1.3 ADC中断服务函数

```c
void ADC_IRQHandler(void)
{
    if(ADC_GetITStatus(ADCx, ADC_IT_EOC) != RESET) // 转换完成中断
    {
        ADC_ConverValue = ADC_GetConversionValue(ADCx); // 获取转换值
        LED_TOGGLE(BLUE_LED_Pin);
    }   
    ADC_ClearITPendingBit(ADCx, ADC_IT_EOC); // 清中断标志位
}
```

#### 3.1.4 主函数测试

```c
#include "stm32f10x.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "adc.h"
#include <stdio.h>

extern __IO uint16_t ADC_ConverValue;
float ADC_Result;

int main(void) 
{
    BSP_LED_Init();
    SysTick_Init();
    USARTx_Init();
    ADC_Mode_Init();
    while(1) {
        ADC_Result = (float)ADC_ConverValue/4096*3.3;
        printf("ADC_ConverValue = 0x%04x \r\n", ADC_ConverValue);
        printf("ADC_Result = %.2f \r\n", ADC_Result);
        Delay_ms(2000);
    }
}
```

### 3.2 独立模式多通道采集（DMA）

#### 3.2.1 ADC相关参数宏定义

```c
#ifndef __ADC_H__
#define __ADC_H__ 

#include "stm32f10x.h"

#define ADCx                   ADC1
#define ADC_GPIO_PORT          GPIOC
#define ADC_DMA_CHANNEL        DMA1_Channel1
#define CH_Num                 6

// 引脚到通道的映射
#define ADC_PINS (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5)
#define ADC_CHANNELS {ADC_Channel_10, ADC_Channel_11, ADC_Channel_12, \
                      ADC_Channel_13, ADC_Channel_14, ADC_Channel_15}

void ADCx_DMA_Init(void);
extern __IO uint16_t ADC_ConverValue[CH_Num];

#endif /* __ADC_H__ */
```

#### 3.2.2 ADC模式配置

```c
#include "adc.h"

__IO uint16_t ADC_ConverValue[CH_Num];
const uint8_t adc_channels[CH_Num] = ADC_CHANNELS;

static void ADC_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
    GPIO_InitStruct.GPIO_Pin = ADC_PINS;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void ADC_Mode_Init(void) {
    DMA_InitTypeDef DMA_InitStruct = {0};
    ADC_InitTypeDef ADC_InitStruct = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // DMA配置
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)ADC_ConverValue;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize = CH_Num;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStruct);
    DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);

    // ADC配置
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = ENABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = CH_Num;
    ADC_Init(ADCx, &ADC_InitStruct);

    // 配置每个通道
    for(uint8_t i=0; i<CH_Num; i++) {
        ADC_RegularChannelConfig(ADCx, adc_channels[i], i+1, ADC_SampleTime_55Cycles5);
    }

    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    ADC_DMACmd(ADCx, ENABLE);
    ADC_Cmd(ADCx, ENABLE);

    // 校准
    ADC_ResetCalibration(ADCx);
    while(ADC_GetResetCalibrationStatus(ADCx));
    ADC_StartCalibration(ADCx);
    while(ADC_GetCalibrationStatus(ADCx));

    ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}

void ADCx_DMA_Init(void) {
    ADC_GPIO_Init();
    ADC_Mode_Init();
}
```

#### 3.2.3 主函数测试

```c
#include "stm32f10x.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "adc.h"
#include <stdio.h>

extern __IO uint16_t ADC_ConverValue[CH_Num];
float ADC_Result[CH_Num];

int main(void) 
{
    BSP_LED_Init();
    SysTick_Init();
    USARTx_Init();
    ADCx_DMA_Init();
    while(1) {
        ADC_Result[0] = (float)ADC_ConverValue[0]/4096*3.3;
        ADC_Result[1] = (float)ADC_ConverValue[1]/4096*3.3; 
        ADC_Result[2] = (float)ADC_ConverValue[2]/4096*3.3;
        ADC_Result[3] = (float)ADC_ConverValue[3]/4096*3.3;
        ADC_Result[4] = (float)ADC_ConverValue[4]/4096*3.3;
        ADC_Result[5] = (float)ADC_ConverValue[5]/4096*3.3;
        printf("\r\n CH0(PC0) value = %f V \r\n",ADC_Result[0]);
        printf("\r\n CH1(PC1) value = %f V \r\n",ADC_Result[1]);
        printf("\r\n CH2(PC2) value = %f V \r\n",ADC_Result[2]);
        printf("\r\n CH3(PC3) value = %f V \r\n",ADC_Result[3]);
        printf("\r\n CH4(PC4) value = %f V \r\n",ADC_Result[4]);
        printf("\r\n CH5(PC5) value = %f V \r\n",ADC_Result[5]);
        Delay_ms(2000);    
    }
}
```

### 3.3 双ADC（同步规则）

![屏幕截图 2025-07-01 143220.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/07/01-14-32-35-屏幕截图%202025-07-01%20143220.png)

#### 3.3.1 ADC相关参数宏定义

```c
#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

#define ADC1_CHANNEL ADC_Channel_11
#define ADC2_CHANNEL ADC_Channel_14

#define ADC1_GPIO_PORT GPIOC
#define ADC1_GPIO_PIN  GPIO_Pin_1
#define ADC2_GPIO_PORT GPIOC
#define ADC2_GPIO_PIN  GPIO_Pin_4

#define ADC_DMA_CHANNEL DMA1_Channel1

void ADCx_Init(void);
extern __IO uint32_t ADC_ConvertedValue; // 双ADC模式结果存储

#endif /* __ADC_H */

```

#### 3.3.2 ADC模式配置

```c
#include "ADC.h"

// 双ADC模式转换结果 (ADC2结果在高16位，ADC1结果在低16位)
__IO uint32_t ADC_ConvertedValue = 0;

static void ADC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // 使能GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    // 配置ADC1引脚 (PC1)
    GPIO_InitStruct.GPIO_Pin = ADC1_GPIO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入模式
    GPIO_Init(ADC1_GPIO_PORT, &GPIO_InitStruct);
    
    // 配置ADC2引脚 (PC4)
    GPIO_InitStruct.GPIO_Pin = ADC2_GPIO_PIN;
    GPIO_Init(ADC2_GPIO_PORT, &GPIO_InitStruct);
}

static void ADC_Mode_Init(void)
{
    DMA_InitTypeDef DMA_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;
    
    // 1. 时钟使能
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);      // DMA1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);   // ADC1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);   // ADC2时钟
    
    // 2. DMA配置
    DMA_DeInit(ADC_DMA_CHANNEL);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR); // 外设地址(ADC1数据寄存器)
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue; // 内存地址
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;         // 外设到内存
    DMA_InitStruct.DMA_BufferSize = 1;                      // 传输数据量(1个32位数据)
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址固定
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;   // 内存地址固定
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // 32位传输
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;   // 32位传输
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;            // 循环模式
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;        // 高优先级
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;               // 禁用内存到内存
    DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStruct);
    DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);                      // 使能DMA
    
    // 3. ADC公共配置
    ADC_InitStruct.ADC_ScanConvMode = ENABLE;               // 扫描模式
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;         // 连续转换
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;     // 数据右对齐
    ADC_InitStruct.ADC_NbrOfChannel = 1;                    // 1个转换通道
    
    // 4. ADC1配置 (主ADC)
    ADC_InitStruct.ADC_Mode = ADC_Mode_RegSimult;           // 同步规则模式
    ADC_Init(ADC1, &ADC_InitStruct);
    ADC_RegularChannelConfig(ADC1, ADC1_CHANNEL, 1, ADC_SampleTime_239Cycles5); // 通道11
    ADC_DMACmd(ADC1, ENABLE);                               // 使能DMA
    
    // 5. ADC2配置 (从ADC)
    ADC_InitStruct.ADC_Mode = ADC_Mode_RegSimult;           // 同步规则模式
    ADC_Init(ADC2, &ADC_InitStruct);
    ADC_RegularChannelConfig(ADC2, ADC2_CHANNEL, 1, ADC_SampleTime_239Cycles5); // 通道14
    ADC_ExternalTrigConvCmd(ADC2, ENABLE);                  // 使能外部触发
    
    // 6. 时钟配置 (8分频 = 72MHz/8 = 9MHz)
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    
    // 7. 校准ADC
    // ADC1校准
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    
    // ADC2校准
    ADC_Cmd(ADC2, ENABLE);
    ADC_ResetCalibration(ADC2);
    while(ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2));
    
    // 8. 启动转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 启动ADC1转换(同步触发ADC2)
}

void ADCx_Init(void)
{
    ADC_GPIO_Init();
    ADC_Mode_Init();
}

```

#### 3.3.3 主函数测试

```c
#include "stm32f10x.h"
#include "ADC.h"
#include "SysTick.h"
#include "usart.h"

// 双ADC模式结果变量 (ADC2结果在高16位，ADC1结果在低16位)
extern __IO uint32_t ADC_ConvertedValue;

int main(void)
{
    ADCx_Init();        
    USARTx_Init();   
    SysTick_Init();   
    // 电压转换结果
    float ADC1_Voltage = 0.0f;
    float ADC2_Voltage = 0.0f;  
    while(1)
    {
        // 从32位转换值中提取两个ADC的结果
        uint16_t adc2_value = (ADC_ConvertedValue >> 16) & 0xFFFF; // 高16位: ADC2
        uint16_t adc1_value = ADC_ConvertedValue & 0xFFFF;         // 低16位: ADC1
        // 转换为电压值 (3.3V参考电压, 12位ADC)
        ADC1_Voltage = (adc1_value * 3.3f) / 4096.0f;
        ADC2_Voltage = (adc2_value * 3.3f) / 4096.0f;
        printf("ADC1 (PC1) Voltage = %.3f V\r\n", ADC1_Voltage);
        printf("ADC2 (PC4) Voltage = %.3f V\r\n", ADC2_Voltage);
        Delay_ms(5000);
    }
}

```

## 4. ADC常见函数（STD库）

### 4.1 ADC 配置步骤

#### 4.1.1使能时钟

```c
RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
```

#### 4.1.2 配置GPIO引脚

```c
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;         // PA0为ADC通道0
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;     // 模拟输入模式
GPIO_Init(GPIOA, &GPIO_InitStruct);
```

#### 4.1.3 ADC参数配置

```c
ADC_InitTypeDef ADC_InitStruct;
ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;       // 独立模式
ADC_InitStruct.ADC_ScanConvMode = DISABLE;            // 单通道禁用扫描
ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;      // 单次转换模式
ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;   // 数据右对齐
ADC_InitStruct.ADC_NbrOfChannel = 1;                  // 转换通道数

ADC_Init(ADC1, &ADC_InitStruct);  // 应用配置
```

#### 4.1.4 通道配置

```c
// 配置通道0的采样时间（239.5周期）
ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
```

#### 4.1.5 使能ADC

```c
ADC_Cmd(ADC1, ENABLE);  // 使能ADC1

// 校准ADC（必须步骤）
ADC_ResetCalibration(ADC1);
while (ADC_GetResetCalibrationStatus(ADC1));
ADC_StartCalibration(ADC1);
while (ADC_GetCalibrationStatus(ADC1));
```

### 4.2 核心操作函数

#### 4.2.1 单通道单次转换

```c
uint16_t ADC_Read(ADC_TypeDef* ADCx, uint8_t channel) {
    ADC_RegularChannelConfig(ADCx, channel, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADCx, ENABLE);             // 启动转换
    while (!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC));     // 等待转换完成
    return ADC_GetConversionValue(ADCx);                // 读取结果
}
```

#### 4.2.2 多通道扫描转换

```c
// 配置多个通道（通道0、1、2）
ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);
ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_239Cycles5);

// 启用扫描模式
ADC_InitStruct.ADC_ScanConvMode = ENABLE;
ADC_InitStruct.ADC_NbrOfChannel = 3;
ADC_Init(ADC1, &ADC_InitStruct);

// 启动转换
ADC_SoftwareStartConvCmd(ADC1, ENABLE);
while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));  // 等待所有通道转换完成
```

#### 4.2.3 连续转换模式

```c
ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
ADC_Init(ADC1, &ADC_InitStruct);

// 启动连续转换
ADC_SoftwareStartConvCmd(ADC1, ENABLE);
```

### 4.3 DMA 配置

#### 4.3.1 使能ADC DMA请求

```c
ADC_DMACmd(ADC1, ENABLE);
```

#### 4.3.2 配置DMA通道

```c
// ADC1使用DMA1通道1
DMA_InitTypeDef DMA_InitStruct;
DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; // ADC数据寄存器地址
DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)adc_buffer;    // 内存缓冲区
DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;              // 外设->内存
DMA_InitStruct.DMA_BufferSize = BUFFER_SIZE;                 // 数据量
DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;          // 内存地址递增
DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 16位
DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;                 // 循环模式
DMA_InitStruct.DMA_Priority = DMA_Priority_High;
DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;

DMA_Init(DMA1_Channel1, &DMA_InitStruct);
DMA_Cmd(DMA1_Channel1, ENABLE);
```

### 4.4 内部通道使用

#### 4.4.1 温度传感器

```c
// 使能内部温度传感器
ADC_TempSensorVrefintCmd(ENABLE);

// 配置通道16（温度传感器）
ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);

// 读取温度值（需校准）
uint16_t temp_value = ADC_Read(ADC1, ADC_Channel_16);
float temperature = ((1.43 - (temp_value * 3.3 / 4095)) / 0.0043) + 25;
```

#### 4.4.2 内部参考电压

```c
// 配置通道17（Vrefint）
ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_239Cycles5);

// 读取参考电压值
uint16_t vref_value = ADC_Read(ADC1, ADC_Channel_17);
```

### 4.5 中断配置

#### 4.5.1 使能中断

```c
// 使能转换完成中断
ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

// 配置NVIC
NVIC_InitTypeDef NVIC_InitStruct;
NVIC_InitStruct.NVIC_IRQChannel = ADC1_2_IRQn;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStruct);
```

#### 4.5.2 中断服务函数

```c
void ADC1_2_IRQHandler(void) {
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) {
        uint16_t adc_value = ADC_GetConversionValue(ADC1);
        // 处理ADC值...
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    }
}
```
