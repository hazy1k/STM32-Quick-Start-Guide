# 第四十五章 ADC-芯片温度

## 1. 硬件设计

使用ADC的内部温度通道获取芯片温度

通过ADC1的通道16获取芯片内部的温度，把获取到的温度通过串口打印到电脑的串口调试助手显示。

均使用芯片资源，无需其他硬件设计

## 2. 软件设计

### 2.1 编程大纲

1. 温度参数宏定义

2. ADC采集内部温度传感器

3. 主函数测试

### 2.2 代码分析

#### 2.2.1 温度参数宏定义

```c
// 12位的ADC，3.3V的ADC值为0xFFF, 温度25°时对应的电压值为1.43V即0x6EE
#define V25 0x6EE
// 斜率每摄氏度4.3mv，对应每摄氏度0x05
#define ADC_K 0x05
#define ADC1_DR_Address ((uint32_t)ADC1_BASE+0x4c)
```

#### 2.2.2 ADC模式配置

```c
#include "ADC.h"

__IO uint16_t ADC_ConvertedValue;

static void ADC_GPIO_Init(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
}

static void ADC_Mode_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    /* DMA1 channel1 configuration */
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);
    /* ADC1 configuration */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);
    ADC_TempSensorVrefintCmd(ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void ADCx_Init(void)
{
    ADC_GPIO_Init();
    ADC_Mode_Init();
}
```

#### 2.2.3 主函数测试

```c
#include "stm32f10x.h"
#include "ADC.h"
#include "usart.h"
#include "SysTick.h"

extern __IO uint16_t ADC_ConvertedValue;
uint16_t ADC_Result;
uint16_t ADC_Result1;

int main(void)
{
    ADCx_Init();
    USART_Config();
    SysTick_Init();
    while(1)
    {
        Delay_ms(2000);
        ADC_Result1 = (V25-ADC_ConvertedValue)/ADC_K+25;
        ADC_Result = (1.43-ADC_ConvertedValue*3.3/4096)*1000/4.3+25;
        printf("ADC内部传感器温度: %3d °C\r\n", ADC_Result);
        printf("ADC Result: %d °C\r\n",ADC_Result1);
    }
}
```

## 3. 小结

### 3.1 实验步骤

#### 1. 硬件准备

- STM32F103系列开发板
- USB连接线

#### 2. 软件准备

- 开发环境: Keil, IAR 或其他支持STM32的IDE

#### 3. 项目配置

1. **引入标准库**: 确保你已引入STM32标准外设库。
2. **配置ADC和DMA**: 通过寄存器设置来配置ADC和DMA。

### 3.2 代码示例

```c
#include "stm32f10x.h" // 引入STM32标准库头文件

#define BUFFER_SIZE 10 // 定义DMA缓冲区大小
uint16_t temperatureBuffer[BUFFER_SIZE]; // 温度数据缓冲区

// 系统时钟配置
void RCC_Configuration(void) {
    // 使能ADC1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    // 使能DMA1的时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

// GPIO配置
void GPIO_Configuration(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // 设置ADC输入引脚（PA0）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 设置为模拟输入模式
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIOA
}

// ADC配置
void ADC_Configuration(void) {
    ADC_InitTypeDef ADC_InitStructure; // ADC初始化结构体
    ADC_CommonInitTypeDef ADC_CommonInitStructure; // ADC公共初始化结构体

    // ADC工作模式配置
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; // 独立模式
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2; // 时钟分频
    ADC_CommonInit(&ADC_CommonInitStructure); // 进行ADC公共配置

    // ADC具体初始化配置
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; // 12位分辨率
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; // 禁用扫描转换模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // 启用连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 无外部触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1; // 通道数量为1
    ADC_Init(ADC1, &ADC_InitStructure); // 初始化ADC1

    // 启用内部温度传感器和参考电压
    ADC_TempSensorVrefintCmd(ENABLE); 
    ADC_Cmd(ADC1, ENABLE); // 启动ADC1
}

// DMA配置
void DMA_Configuration(void) {
    DMA_InitTypeDef DMA_InitStructure; // DMA初始化结构体
    // 设置DMA的外设基地址（ADC数据寄存器地址）
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; 
    // 设置DMA的内存基地址（数据缓冲区）
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)temperatureBuffer; 
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // 设置DMA传输方向为外设到内存
    DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE; // 设置DMA缓冲区大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 禁用外设地址递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 启用内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 外设数据大小为半字（16位）
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 内存数据大小为半字（16位）
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 设置为循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; // 设置DMA优先级为高
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); // 初始化DMA1通道1
    DMA_Cmd(DMA1_Channel1, ENABLE); // 启用DMA1通道1
}

// 启动ADC转换
void ADC_Start(void) {
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 软件启动ADC转换
}

// 主函数
int main(void) {
    RCC_Configuration(); // 调用时钟配置函数
    GPIO_Configuration(); // 调用GPIO配置函数
    DMA_Configuration(); // 调用DMA配置函数
    ADC_Configuration(); // 调用ADC配置函数
    ADC_Start(); // 启动ADC转换

    while (1) {
        // 在这里处理温度数据
        // 例如，可以读取temperatureBuffer中的数据并进行处理
    }
}

// DMA中断处理程序
void DMA1_Channel1_IRQHandler(void) {
    // 在这里处理DMA中断
    // 例如，可以处理接收到的数据
}
```

---

2024.9.21 第一次修订，后期不再维护

2025.1.30 修订代码
