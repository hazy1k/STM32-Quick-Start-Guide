# 第42章 ADC-单通道（DMA读取）

## 1. 硬件设计

和单通道(中断读取)的实验差不多，差别在于ADC转换结果数据使用DMA方式传输到指定的存储区，取代单通道使用中断的读取方法

## 2. 软件分析

### 2.1 编程大纲

1. ADC相关参数宏定义

2. ADC及DMA配置

3. 主函数测试

### 2.2 代码分析

#### 2.2.1 ADC GOIO及通道宏定义

```c
/* ADC通道相关参数定义 */
#define ADCx ADC1
#define ADC_APBxClock RCC_APB2PeriphClockCmd
#define ADC_CLK RCC_APB2Periph_ADC1
#define ADC_GPIO_APBxClock RCC_APB2PeriphClockCmd
#define ADC_GPIO_CLK RCC_APB2Periph_GPIOC
#define ADC_GPIO_PORT GPIOC
#define ADC_GPIO_PIN GPIO_Pin_1
#define ADC_CHANNEL ADC_Channel_11
#define ADC_DMA_CHANNEL DMA1_Channel1
```

#### 2.2.2 ADC及DMA模式配置

```c
#include "ADC.h"

__IO uint16_t ADC_ConvertedValue;

static void ADC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_GPIO_APBxClock(ADC_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入模式
    GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
}

static void ADC_Mode_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    /* 时钟配置 */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    ADC_APBxClock(ADC_CLK, ENABLE);
    /* DAM配置 */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx->DR)); // 外设地址为ADC数据寄存器地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue; // 内存地址为ADC_ConvertedValue变量地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // 数据源来自外设，也就是ADC
    DMA_InitStructure.DMA_BufferSize = 1; // 一次只传输一个数据
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不自增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; // 内存地址不自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
    DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
    /* ADC配置 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADCx, &ADC_InitStructure);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8); // 设置ADC时钟为SYSCLK/8 = 72/8 = 9MHz
    ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5); // 配置ADC通道，选择ADC_CHANNEL，转换时间为55个时钟周期
    ADC_Cmd(ADCx, ENABLE);
    ADC_ResetCalibration(ADCx);
    while(ADC_GetResetCalibrationStatus(ADCx));
    ADC_StartCalibration(ADCx);
    while(ADC_GetCalibrationStatus(ADCx));
    ADC_SoftwareStartConvCmd(ADCx, ENABLE);
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
#include "usart.h"
#include "ADC.h"
#include "SysTick.h"

extern __IO uint16_t ADC_ConvertedValue;
float ADC_Result;

int main(void)
{
    SysTick_Init();
    USART_Config();
    ADCx_Init();
    while (1)
    {
        // 检查ADC是否完成转换
        if(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC))
        {
            // 如果DMA启用，直接从内存变量获取值
            ADC_ConvertedValue = ADC_GetConversionValue(ADCx); // 读取转换结果
            ADC_ClearFlag(ADCx, ADC_FLAG_EOC);  // 清除标志位
            ADC_Result = (float)ADC_ConvertedValue / 4096 * 3.3;
            printf("\r\n ADC采集到的值 = 0x%04X \r\n", ADC_ConvertedValue);
            printf("\r\n 计算得到的电压值 = %f V \r\n", ADC_Result);
            Delay_ms(1000); 
        }
        Delay_ms(2000);
    }
}
```

## 3. 小结

这一章主要就增加了一个DMA传输嘛，DMA我们前面就已经学习过了，所以还算比较简单的

### 3.1 实验目标

使用 STM32 标准库通过 DMA 读取电位器的电压值。

### 3.2 硬件需求

与之前相同：

1. STM32 开发板（如 STM32F103）
2. 电位器
3. 连接线
4. 电源

### 3.3 硬件连接

- 电位器的一个引脚接到 VCC（如 3.3V），另一个引脚接到 GND，中间引脚接到 ADC 输入通道（如 PA0）。

### 3.4 软件设计

1. **项目设置**
   
   - 在 STM32 标准库中创建新项目，配置 ADC 模块和 DMA。

2. **代码实现**

#### 主程序代码

```c
#include "stm32f10x.h"

#define ADC_BUFFER_SIZE  10
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];

void ADC1_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  // PA0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  // 模拟输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // 开启连续转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Cmd(ADC1, ENABLE);  // 启动 ADC

    // DMA 初始化
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  // 使能 DMA 时钟

    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adc_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  // 从外设到内存
    DMA_InitStructure.DMA_BufferSize = ADC_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  // 循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);  // 启动 DMA

    // 启动 ADC
    ADC_DMACmd(ADC1, ENABLE);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);  // 启动软件转换
}

int main(void) {
    SystemInit();
    ADC1_Init();

    while (1) {
        // 在这里可以处理 adc_buffer 中的数据
        for (int i = 0; i < ADC_BUFFER_SIZE; i++) {
            // 处理 adc_buffer[i]
        }
        // 添加适当的延时
    }
}
```

---

2024.9.20 第一次修订，后期不再维护

2025.1.30 修订代码
