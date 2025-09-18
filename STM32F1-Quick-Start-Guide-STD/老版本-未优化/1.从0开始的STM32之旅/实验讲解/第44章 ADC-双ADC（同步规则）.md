# 第四十四章 ADC-双ADC（同步规则）

## 1. 双ADC介绍

AD转换包括采样阶段和转换阶段，在采样阶段才对通道数据进行采集；而在转换阶段只是将采集到的数据进行转换为数字量输出，此刻通道数据变化不会改变转换结果。

独立模式的ADC采集需要在一个通道采集并且转换完成后才会进行下一个通道的采集。而双重ADC的机制就是使用两个ADC同时采样一个或者多个通道。 双重ADC模式较独立模式一个最大的优势就是提高了采样率，弥补了单个ADC采样不够快的缺点。

启用双ADC模式的时候，通过配置ADC_CR1寄存器的DUALMOD[3:0]位，可以有几种不同的模式， 具体见表格：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/ADC01.png)

这里我们选取同步规则模式来作为实验讲解。同步规则模式是ADC1和ADC2同时转换一个规则通道组，ADC1是主，ADC2是从， ADC1转换的结果放在ADC1_DR的低16位，ADC2转换的结果放在ADC1_DR的高十六位。并且必须开启DMA功能。

外部触发来自ADC1的规则组多路开关(由ADC1_CR2寄存器的EXTSEL[2:0]选择)， 它同时给ADC2提供同步触发。 为了简单起见，ADC1跟ADC2我们选择软件触发。

为了实验的简单起见，实验中我们选取ADC1和ADC2各采集一个通道 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/ADC007.png)

## 2. 硬件设计

开发板用引出了6路ADC，实验中选取其中的两路（根据软件配置来选择）接开发板中的电位器/GND/3V3来获取模拟信号即可做实验。

## 3. 软件设计

### 3.1 编程大纲

1) ADC1和2相关参数的回忆

2) 配置ADC

3) 读取ADC采集的数据，并打印出来校正；

### 2.2 代码分析

#### 2.2.1 ADC宏定义

```c
#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

/* ADC1通道 */
#define ADCx_1 ADC1
#define ADCx_1_APBxClock RCC_APB2PeriphClockCmd
#define ADCx_1_CLK RCC_APB2Periph_ADC1
#define ADCx_1_GPIO_APBxClock RCC_APB2PeriphClockCmd
#define ADCx_1_GPIO_CLK RCC_APB2Periph_GPIOC
#define ADCx_1_GPIO_PORT GPIOC
#define ADCx_1_GPIO_PIN GPIO_Pin_1
#define ADCx_1_CHANNEL ADC_Channel_11
/* ADC2通道 */
#define ADCx_2 ADC2
#define ADCx_2_APBxClock RCC_APB2PeriphClockCmd
#define ADCx_2_CLK RCC_APB2Periph_ADC2
#define ADCx_2_GPIO_APBxClock RCC_APB2PeriphClockCmd
#define ADCx_2_GPIO_CLK RCC_APB2Periph_GPIOC
#define ADCx_2_GPIO_PORT GPIOC
#define ADCx_2_GPIO_PIN GPIO_Pin_4
#define ADCx_2_CHANNEL ADC_Channel_14
/* DMA通道 */
#define ADC_DMA_CHANNEL DMA1_Channel1

void ADCx_Init(void);

#endif /* __ADC_H */

```

ADC1和ADC2的通道由宏来定义，当硬件环境改变时，方便修改。

#### 2.2.2 ADC初始化

```c
#include "ADC.h"

__IO uint16_t ADC_ConvertedValue[1] = {0};

static void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* ADC1 GPIO Configuration */
    ADCx_1_GPIO_APBxClock(ADCx_1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADCx_1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(ADCx_1_GPIO_PORT, &GPIO_InitStructure);
	/* ADC2 GPIO Configuration */
	ADCx_2_GPIO_APBxClock(ADCx_2_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADCx_2_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(ADCx_2_GPIO_PORT, &GPIO_InitStructure);
}

static void ADC_Mode_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	/* 时钟配置 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	ADCx_1_APBxClock(ADCx_1_CLK, ENABLE);
	ADCx_2_APBxClock(ADCx_2_CLK, ENABLE);
	/* DMA 配置 */
	DMA_DeInit(ADC_DMA_CHANNEL);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx_1->DR));// 基地址ADC1数据寄存器
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue; // 传输地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
	/* ADC1 配置 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult; // 双ADC规则同步
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; // 扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // 连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 外部触发转换禁止
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADCx_1, &ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	ADC_RegularChannelConfig(ADCx_1, ADCx_1_CHANNEL, 1, ADC_SampleTime_239Cycles5);
	ADC_DMACmd(ADCx_1, ENABLE);
	/* ADC2 配置 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADCx_2, &ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	ADC_RegularChannelConfig(ADCx_2, ADCx_2_CHANNEL, 1, ADC_SampleTime_239Cycles5);
	ADC_ExternalTrigConvCmd(ADCx_2, ENABLE); // 外部触发转换使能
	/* ADC 校准*/
	ADC_Cmd(ADCx_1, ENABLE);
	ADC_ResetCalibration(ADCx_1);
	while(ADC_GetResetCalibrationStatus(ADCx_1));
	ADC_StartCalibration(ADCx_1);
	while(ADC_GetCalibrationStatus(ADCx_1));
	ADC_Cmd(ADCx_2, ENABLE);
	ADC_ResetCalibration(ADCx_2);
	while(ADC_GetResetCalibrationStatus(ADCx_2));
	ADC_StartCalibration(ADCx_2);
	while(ADC_GetCalibrationStatus(ADCx_2));
	ADC_SoftwareStartConvCmd(ADCx_1, ENABLE); // 软件启动转换
}

void ADCx_Init(void)
{
	ADC_GPIO_Init();
	ADC_Mode_Init();
}


```

#### 2.2.3 主函数

```c
#include "stm32f10x.h"
#include "ADC.h"
#include "SysTick.h"
#include "usart.h"
// 双模式时，ADC1和ADC2转换的数据都存放在ADC1的数据寄存器，
// ADC1的在低十六位，ADC2的在高十六位
extern __IO uint32_t ADC_ConvertedValue[1];
float ADC_Result[2];
int main(void)
{
	ADCx_Init();
	USART_Config();
	SysTick_Init();
	uint16_t temp_h = 0, temp_l = 0;
	while(1)
	{
		temp_h = (ADC_ConvertedValue[0]&0xFFFF0000) >> 16;
		temp_l = (ADC_ConvertedValue[0]&0xFFFF);
		ADC_Result[0] = (float)temp_h/4096*3.3;
		ADC_Result[1] = (float)temp_l/4096*3.3;
		printf("ADC_1 of Vref = %f V\r\n", ADC_Result[0]);
		printf("ADC_2 of Vref = %f V\r\n", ADC_Result[1]);
		ADC_ClearFlag(ADCx_1, ADC_FLAG_EOC);
		Delay_ms(5000);
	}
}

```

配置好串口，初始化好ADC，然后把ADC1和ADC2采集的数据分离出来，最后调用printf函数打印到电脑的串口调试助手。

## 3. 小结

相比单通道，双ADC采用同步规则，提高采集精度，ADC1软件触发，ADC2外部触发

1. **初始化ADC**：
   配置两个ADC（ADC1和ADC2），设置为同步工作模式。

2. **设置规则通道**：
   配置两个ADC的规则通道，使其共享相同的输入通道。

3. **配置DMA**：
   使用DMA将两个ADC的采样结果同时存储，以提高效率。

```c
void ADC_Dual_Init(void) {
    // 省略时钟和GPIO初始化部分

    ADC_InitTypeDef ADC_InitStruct;

    // 初始化ADC1
    ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStruct.ADC_ScanConvMode = ENABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = 2; // 两个通道
    ADC_Init(ADC1, &ADC_InitStruct);

    // 初始化ADC2
    ADC_Init(ADC2, &ADC_InitStruct);

    // 配置ADC通道
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1_5Cycles);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 1, ADC_SampleTime_1_5Cycles);

    // 启用ADC
    ADC_Cmd(ADC1, ENABLE);
    ADC_Cmd(ADC2, ENABLE);

    // 启动ADC转换
    ADC_StartOfConversion(ADC1);
    ADC_StartOfConversion(ADC2);
}

// 在主循环中读取数据
void Read_ADC_Dual(void) {
    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != RESET) {
        uint16_t value1 = ADC_GetConversionValue(ADC1);
        uint16_t value2 = ADC_GetConversionValue(ADC2);
        // 处理数据
    }
}
```

---

2024.9.22 第一次修订，后期不再维护

2025.1.30 修订代码
