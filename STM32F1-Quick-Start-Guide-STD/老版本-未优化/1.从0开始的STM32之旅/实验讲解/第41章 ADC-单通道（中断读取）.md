# 第41章 ADC-单通道（中断读取）

## 1. 硬件设计

开发板板载一个贴片滑动变阻器，电路设计见图：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/ADC006.png)

贴片滑动变阻器的动触点通过连接至STM32芯片的ADC通道引脚。当我们使用旋转滑动变阻器调节旋钮时， 其动触点电压也会随之改变，电压变化范围为0~3.3V，亦是开发板默认的ADC电压采集范围。

## 2. 软件设计

### 2.1 编程大纲

1. ADC相关参数宏定义

2. ADC单通道中断采集

3. 中断服务函数

4. 主函数测试

### 2.2 代码分析

#### 2.2.1 ADC宏定义

```c
/* ADC通道相关参数定义 */
#define ADC_GPIO_APBxClock RCC_APB2PeriphClockCmd
#define ADC_GPIO_CLK RCC_APB2Periph_GPIOC
#define ADC_GPIO_PORT GPIOC
#define ADC_GPIO_PIN GPIO_Pin_1
#define ADC_APBxClock RCC_APB2PeriphClockCmd
#define ADCx ADC2
#define ADC_CLK RCC_APB2Periph_ADC2
#define ADC_CHANNEL ADC_Channel_11
/* 中断相关 */
#define ADC_IRQ ADC1_2_IRQn
#define ADC_IRQHandler ADC1_2_IRQHandler
```

使用宏定义引脚信息方便硬件电路改动时程序移植。

#### 2.2.2 配置ADC工作模式

```c
#include "ADC.h"

__IO uint16_t ADC_ConvertedValue; // 全局变量，存放ADC转换值

static void ADC_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void ADC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_GPIO_APBxClock(ADC_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 单通道输入模式
    GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
}

static void ADC_Mode_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    ADC_APBxClock(ADC_CLK, ENABLE);
    /* ADC模式配置 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // 独立模式，只用到了一个ADC通道
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; // 不使用扫描模式，多通道才使用
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // 连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 禁止外部触发，使用软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1; // 单通道模式
    ADC_Init(ADCx, &ADC_InitStructure);
    /* ADC时钟配置 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8); // 设置ADC时钟为PLCK2的8分频，即72/8=9MHz
    ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5); // 配置ADC通道，采样时间为55.5个时钟周期
    /* ADC中断配置 */
    ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE); // 转换结果产生时发生中断，在服务函数中计算转换值
    ADC_Cmd(ADCx, ENABLE); // 使能ADC
    ADC_ResetCalibration(ADCx); // 复位校准寄存器
    while(ADC_GetResetCalibrationStatus(ADCx)); // 等待校准完成
    ADC_StartCalibration(ADCx); // 开始校准
    while(ADC_GetCalibrationStatus(ADCx)); // 等待校准完成
    ADC_SoftwareStartConvCmd(ADCx, ENABLE); // 启动软件转换
}

void ADCx_Init(void)
{
    ADC_NVIC_Init();
    ADC_GPIO_Init();
    ADC_Mode_Init();
}
```

首先，定义一个ADC初始化结构体ADC_InitTypeDef，用来配置ADC具体的工作模式。然后调用RCC_APB2PeriphClockCmd()开启ADC时钟。

ADC工作参数具体配置为：独立模式、单通道采集不需要扫描、启动连续转换、使用内部软件触发无需外部触发事件、使用右对齐数据格式、 转换通道为1，并调用ADC_Init函数完成ADC1工作环境配置。

RCC_ADCCLKConfig()函数用来配置ADC的工作时钟，接收一个参数，设置的是PCLK2的分频系数，ADC的时钟最大不能超过14M。

ADC_RegularChannelConfig函数用来绑定ADC通道的转换顺序和时间。它接收4个形参，第一个形参选择ADC外设，可为ADC1、ADC2或ADC3； 第二个形参通道选择，总共可选18个通道；第三个形参为通道的转换顺序，可选为1到16；第四个形参为采样周期选择，采样周期越短， ADC转换数据输出周期就越短但数据精度也越低，采样周期越长，ADC转换数据输出周期就越长同时数据精度越高。

利用ADC转换完成中断可以非常方便的保证我们读取到的数据是转换完成后的数据而不用担心该数据可能是ADC正在转换时“不稳定”的数据。 我们使用ADC_ITConfig函数使能ADC转换完成中断，并在中断服务函数中读取转换结果数据。

最后，在ADC校准之后调用ADC_SoftwareStartConvCmd函数进行软件触发ADC开始转换。

#### 2.2.3 ADC中断服务函数

```c
extern __IO uint16_t ADC_ConvertedValue;
void ADC_IRQHandler(void)
{    
    if(ADC_GetITStatus(ADCx, ADC_IT_EOC) == SET)  
    {
        // 读取ADC的转换值
        ADC_ConvertedValue = ADC_GetConversionValue(ADCx);
    }
    ADC_ClearITPendingBit(ADCx,ADC_IT_EOC);
}
```

中断服务函数一般定义在stm32f10x_it.c文件内，我们使能了ADC转换完成中断，在ADC转换完成后就会进入中断服务函数， 我们在中断服务函数内直接读取ADC转换结果保存在变量ADC_ConvertedValue(在main.c中定义)中。

ADC_GetConversionValue函数是获取ADC转换结果值的库函数，只有一个形参为ADC外设，可选为ADC1、ADC2或ADC3， 该函数还返回一个16位的ADC转换结果值。

这个中断处理函数也比较重要，我们需要解释一下：

#### 2.2.4 主函数

```c
#include "stm32f10x.h"
#include "usart.h"
#include "ADC.h"
#include "SysTick.h"

extern __IO uint16_t ADC_ConvertedValue;
float ADC_Result;

int main(void)
{
    USART_Config();
    SysTick_Init();
    ADCx_Init();
    while(1)
    {
        ADC_Result = (float)ADC_ConvertedValue/4096*3.3;
        printf("采集到的ADC值为 = 0x%04x \r\n", ADC_ConvertedValue);
        printf("计算得到电压值 = %.2f V \r\n", ADC_Result);
        Delay_ms(2000);
    }
}
```

## 3. 小结

### 3.1 实验目标

使用 STM32 标准库读取电位器的电压值，并在 ADC 转换完成时通过中断处理程序显示读取的值。

### 3.2 硬件需求

1. STM32 开发板（如 STM32F103）
2. 电位器
3. 连接线
4. 电源

### 3.3 硬件连接

- 电位器的一个引脚接到 VCC（如 3.3V），另一个引脚接到 GND，中间引脚接到 ADC 输入通道（如 PA0）。

### 3.4 软件设计

1. **项目设置**
   
   - 在 STM32 标准库中创建新项目，配置 ADC 模块和中断。

2. **代码实现**

#### 主程序代码

```c
#include "stm32f10x.h"

volatile uint16_t adc_value = 0;

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
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_TempSensorVrefintCmd(ENABLE);  // 启用温度传感器和内部参考电压
    ADC_Cmd(ADC1, ENABLE);  // 启动 ADC
}

void ADC_Start(void) {
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);  // 启动软件转换
}

void ADC_IRQHandler(void) {
    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) {
        adc_value = ADC_GetConversionValue(ADC1);  // 获取转换值
        ADC_ClearFlag(ADC1, ADC_FLAG_EOC);  // 清除转换完成标志
    }
}

int main(void) {
    SystemInit();
    ADC1_Init();

    while (1) {
        ADC_Start();  // 启动 ADC 转换
        // 可以在此处添加延时或其他处理
    }
}
```

# 

---

2024.9.20 第一次修订，后期不再维护

2025.1.30 简化内容
