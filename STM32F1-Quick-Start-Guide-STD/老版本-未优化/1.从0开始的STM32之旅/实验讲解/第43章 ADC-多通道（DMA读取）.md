# 第四十三章 ADC-多通道（DMA读取）

## 1. 硬件设计

开发板已通过排针接口把部分ADC通道引脚引出，其中电位器通过跳线帽默认接了一个ADC的IO， 其他的ADC IO 在做实验的时候可以用杜邦线连接到开发板中的GND或者3V3来获取模拟信号。要注意的是， 用来做ADC输入的IO不能被复用，否则会导致采集到的信号不准确。

## 2. 软件设计

### 2.1 编程大纲

1. ADC相关参数宏定义

2. 六路ADC及DMA配置

3. 主函数测试

### 2.2 代码分析

#### 2.2.1 ADC相关参数宏定义

```c
#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

/* ADC基础配置 */
#define ADC_APBxClock RCC_APB2PeriphClockCmd
#define ADC_CLK RCC_APB2Periph_ADC1
#define ADC_GPIO_APBxClock RCC_APB2PeriphClockCmd
#define ADC_GPIO_CLK RCC_APB2Periph_GPIOC
#define ADC_GPIO_PORT GPIOC
#define ADCx ADC1
#define ADC_DMA_CHANNEL DMA1_Channel1
#define ADC_DMA_CLK RCC_AHBPeriph_DMA1
/* 六路ADC通道配置 */
#define CH_Num 6
#define ADC_PIN1 GPIO_Pin_0
#define ADC_CHANNE1 ADC_Channel_10
#define ADC_PIN2 GPIO_Pin_1
#define ADC_CHANNE2 ADC_Channel_11
#define ADC_PIN3 GPIO_Pin_2
#define ADC_CHANNE3 ADC_Channel_12
#define ADC_PIN4 GPIO_Pin_3
#define ADC_CHANNE4 ADC_Channel_13
#define ADC_PIN5 GPIO_Pin_4
#define ADC_CHANNE5 ADC_Channel_14
#define ADC_PIN6 GPIO_Pin_5
#define ADC_CHANNE6 ADC_Channel_15

void ADCx_Init(void);

#endif /* __ADC_H */

```

#### 2.2.2 ADC及DMA模式配置

```c
#include "ADC.h"

__IO uint16_t ADC_ConvertedValue[CH_Num] = {0,0,0,0,0,0};

static void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_GPIO_APBxClock(ADC_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADC_PIN1|ADC_PIN2|ADC_PIN3|ADC_PIN4|ADC_PIN5|ADC_PIN6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
}

static void ADC_Mode_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	/* 时钟配置 */
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
	ADC_APBxClock(ADC_CLK, ENABLE);
	DMA_DeInit(ADC_DMA_CHANNEL);
	/* DMA 配置 */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx->DR));
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = CH_Num;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
	/* ADC 配置 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = CH_Num;
	ADC_Init(ADCx, &ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	ADC_RegularChannelConfig(ADCx, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_Channel_2, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_Channel_3, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_Channel_4, 4, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_Channel_5, 5, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_Channel_6, 6, ADC_SampleTime_55Cycles5);
	ADC_DMACmd(ADCx, ENABLE);
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

ADCx_Mode_Config函数主要做了两个工作，一个是配置ADC的工作参数，另外一个是配置DMA的工作参数。

ADC的工作参数具体如下：打开ADC外设时钟；因为只是使用一个ADC，所有模式配置为独立模式；多通道采集，开启扫描模式； 需要不断的采集外部的模拟数据，所有使能连续转换模式；不使用外部触发转换信号；转换结果右对齐；设置需要转换的通道的个数， 最后调用ADC_Init()函数把这些参数写入ADC的寄存器完成配置。因为是多通道采集， 所以调用ADC_RegularChannelConfig()函数设置每个通道的转换顺序和采样实际。

DMA的工作参数具体如下：我们是把ADC采集到的数据通过DMA传输到存储器上，则外设地址为ADC的数据寄存器；存储器的地址是我们定义的用来存放ADC数据的数组的地址； 传输方向为外设到存储器；缓冲区大小等于等于我们定义的存储ADC数据的数组大小；所有通道转换的数据都放在一个数据寄存器中，则外设地址不变； 采集存储的数据有多个，则存储器地址递增；外设和存储器单位均为两个字节；开启循环传输模式；只有一个DMA通道工作，优先级随便设置；禁用存储器到存储器模式， 最后最后调用DMA_Init()函数把这些参数写入DMA的寄存器完成配置。

完成配置之后则使能ADC和DMA，开启软件触发，让ADC开始采集数据。

#### 2.2.3 主函数测试

```c
// ADC 6通道采集实验
#include "stm32f10x.h"
#include "usart.h"
#include "adc.h"
#include "SysTick.h"

extern __IO uint16_t ADC_ConvertedValue[CH_Num];	 
float ADC_Result[CH_Num];        

int main(void)
{		
	USART_Config();	
	SysTick_Init();
	ADCx_Init();
	while (1)
	{	
    	ADC_Result[0] = (float)ADC_ConvertedValue[0]/4096*3.3;
	 	ADC_Result[1] = (float)ADC_ConvertedValue[1]/4096*3.3;
		ADC_Result[2] = (float)ADC_ConvertedValue[2]/4096*3.3;
		ADC_Result[3] = (float)ADC_ConvertedValue[3]/4096*3.3;
		ADC_Result[4] = (float)ADC_ConvertedValue[4]/4096*3.3;
		ADC_Result[5] = (float)ADC_ConvertedValue[5]/4096*3.3;
		printf("\r\n CH0(PC0) value = %f V \r\n",ADC_Result[0]);
		printf("\r\n CH1(PC1) value = %f V \r\n",ADC_Result[1]);
		printf("\r\n CH2(PC2) value = %f V \r\n",ADC_Result[2]);
		printf("\r\n CH3(PC3) value = %f V \r\n",ADC_Result[3]);
		printf("\r\n CH4(PC4) value = %f V \r\n",ADC_Result[4]);
		printf("\r\n CH5(PC5) value = %f V \r\n",ADC_Result[5]);
		Delay_ms(5000);		 
	}
}

```

主函数中我们配置好串口，初始化好ADC之后，把采集到的电压经过转换之后通过串口打印到电脑的调试助手显示， 要注意的是在做实验时需要给每个ADC通道提供模拟电源，可以用杜邦线从开发板的GND或者3V3取信号来做实验。

## 3. 小结

这一章就是比上一章多了几个通道而已，把宏定义一加就行了

### 3.1 实验目的

使用STM32F103的ADC模块，通过DMA实现6个通道的数据采集。

### 3.2 硬件连接

- 将6个模拟信号源连接到STM32F103的ADC引脚（PA0~PA5）。

### 3.3 代码示例

```c
#include "stm32f10x.h"

uint16_t adc_values[6]; // 存储ADC采集的6个通道的数据

void ADC_Init(void) {
    ADC_InitTypeDef ADC_InitStruct; // ADC配置结构体
    GPIO_InitTypeDef GPIO_InitStruct; // GPIO配置结构体

    // 启用ADC和GPIO的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // 配置PA0~PA5为模拟输入
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入模式
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // ADC配置
    ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b; // 12位分辨率
    ADC_InitStruct.ADC_ScanConvMode = ENABLE; // 启用扫描转换模式
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE; // 启用连续转换模式
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 无外部触发
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right; // 右对齐数据
    ADC_InitStruct.ADC_NbrOfChannel = 6; // 6个通道
    ADC_Init(ADC1, &ADC_InitStruct);

    // 配置各ADC通道及其采样时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1_5Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_1_5Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_1_5Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_1_5Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_1_5Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_1_5Cycles);

    // 启用ADC
    ADC_Cmd(ADC1, ENABLE);

    // 启动ADC转换
    ADC_StartOfConversion(ADC1);
}

void DMA_Init(void) {
    DMA_InitTypeDef DMA_InitStruct; // DMA配置结构体

    // 启用DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // 配置DMA1通道1
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; // ADC数据寄存器地址
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)adc_values; // 存储ADC数据的数组地址
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC; // 从外设到内存传输
    DMA_InitStruct.DMA_BufferSize = 6; // 缓冲区大小
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不递增
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址递增
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // 外设数据大小为字
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word; // 内存数据大小为字
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular; // 循环模式
    DMA_InitStruct.DMA_Priority = DMA_Priority_High; // 高优先级
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable; // 禁用内存到内存传输
    DMA_Init(DMA1_Channel1, &DMA_InitStruct);

    // 启用DMA1通道1
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

int main(void) {
    ADC_Init(); // 初始化ADC
    DMA_Init(); // 初始化DMA

    while (1) {
        // 主循环，ADC数据自动存储在adc_values数组中
    }
}
```

---

2024.9.22 第一次修订，后期不再维护

2025.1.30 优化代码
