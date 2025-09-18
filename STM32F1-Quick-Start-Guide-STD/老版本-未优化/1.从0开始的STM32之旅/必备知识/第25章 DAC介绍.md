# 第二十五章 DAC介绍

## 1. DAC简介

DAC为数字/模拟转换模块，故名思议，它的作用就是把输入的数字编码，转换成对应的模拟电压输出，它的功能与ADC相反。 在常见的数字信号系统中，大部分传感器信号被化成电压信号，而ADC把电压模拟信号转换成易于计算机存储、处理的数字编码， 由计算机处理完成后，再由DAC输出电压模拟信号，该电压模拟信号常常用来驱动某些执行器件，使人类易于感知。如音频信号的采集及还原就是这样一个过程。

STM32具有片上DAC外设，它的分辨率可配置为8位或12位的数字输入信号，具有两个DAC输出通道，这两个通道互不影响， 每个通道都可以使用DMA功能，都具有出错检测能力，可外部触发。

## 2. DAC功能框图

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/DAC002.jpeg" title="" alt="" width="833">

整个DAC模块围绕框图下方的“数字至模拟转换器x”展开，它的左边分别是参考电源的引脚：V<sub>DDA</sub>、 V<sub>SSA</sub>及V<sub>ref+</sub>， 其中STM32的DAC规定了它的参考电压输入范围为2.4—3.3V。 “数字至模拟转换器x”的输入为DAC的数据寄存器“DORx”的数字编码，经过它转换得的模拟信号由图中右侧的“DAC_OUTx”输出。 而数据寄存器“DORx”又受“控制逻辑”支配，它可以控制数据寄存器加入一些伪噪声信号或配置产生三角波信号。图中的左上角为DAC的触发源， DAC根据触发源的信号来进行DAC转换，其作用就相当于DAC转换器的开关，它可以配置的触发源为外部中断源触发、定时器触发或软件控制触发。 如本章实验中需要控制正弦波的频率，就需要定时器定时触发DAC进行数据转换。

### 2.1 参考电压

与ADC外设类似，DAC也使用V<sub>REF+</sub>引脚作为参考电压， 在设计原理图的时候一般把VSSA接地，把V<sub>REF+</sub>和V<sub>DDA</sub> 接3.3V， 可得到DAC的输出电压范围为：0~3.3V。如果想让输出的电压范围变宽，可以在外部加一个电压调理电路，把0~3.3V的DAC输出抬升到特定的范围即可。

### 2.2 数模转换及输出通道

框图中的“数字至模拟转换器x”是核心部件，整个DAC外设都围绕它而展开。它以左边的V<sub>REF+</sub>作为参考电源， 以DAC的数据寄存器“DORx”的数字编码作为输入，经过它转换得的模拟信号由右侧的“DAC_OUTx”通道输出。其中各个部件中的“x”是指设备的标号， 在STM32中具有2个这样的DAC部件，每个DAC有1个对应的输出通道连接到特定的引脚，即：PA4-通道1，PA5-通道2，为避免干扰，使用DAC功能时， DAC通道引脚需要被配置成模拟输入功能（AIN）。

### 2.3 触发源及DHRx寄存器

在使用DAC时，不能直接对上述DORx寄存器写入数据，任何输出到DAC通道x的数据都必须写入到DHRx寄存器中（其中包含DHR8Rx、DHR12Lx等， 根据数据对齐方向和分辨率的情况写入到对应的寄存器中）。

数据被写入到DHRx寄存器后，DAC会根据触发配置进行处理，若使用硬件触发，则DHRx中的数据会在3个APB1时钟周期后传输至DORx， DORx随之输出相应的模拟电压到输出通道；若DAC设置为外部事件触发，可以使用定时器（TIMx_TRGO）、 EXTI_9信号或软件触发（SWTRIGx）这几种方式控制数据DAC转换的时机，例如使用定时器触发，配合不同时刻的DHRx数据，可实现DAC输出正弦波的功能。

## 3. DAC初始化结构体详解

在ST的标准库中，把控制DAC相关的各种配置封装到了结构体DAC_InitTypeDef中， 它主要包含了DAC_CR控制寄存器的各寄存器位的配置

```c
typedef struct {
    // DAC触发方式 
    uint32_t DAC_Trigger;
    // 是否自动输出噪声或三角波
    uint32_t DAC_WaveGeneration;
    // 选择噪声生成器的低通滤波或三角波的幅值
    uint32_t DAC_LFSRUnmask_TriangleAmplitude;
    // 选择是否使能输出缓冲器
    uint32_t DAC_OutputBuffer;
} DAC_InitTypeDef;
```

各个结构体成员的介绍如下，解说中各模式后括号内的英文为该模式在标准库中使用宏：

(1) DAC_Trigger

> 本成员用于配置DAC的触发模式，当DAC产生相应的触发事件时，才会把DHRx寄存器的值转移到DORx寄存器中进行转换。 本结构体成员可以选择的触发模式如下：硬件触发模式（DAC_Trigger_None），DHRx寄存器内的数据会在3个APB1时钟周期内自动转换至DORx进行转换； 定时器触发模式（DAC_Trigger_T2/4/5/6/7_TRGO），使用定时器2、4、5、6、7控制DHRx寄存器的数据按时间转移到DORx中进行转换， 利用这种方式可以输出特定的波形；EXTI_9触发方式（DAC_Trigger_Ext_IT9），当产生EXTI_9事件时（如GPIO中断事件）， 触发转换；软件触发模式（DAC_Trigger_Software），在本模式下，向DAC_SWTRIGR寄存器写入配置即可触发信号进行转换。

(2) DAC_WaveGeneration

> 本成员用于设置是否使用DAC输出伪噪声或三角波（DAC_WaveGeneration_None/Noise/Triangle），使用伪噪声和三角波输出时， DAC都会把LFSR寄存器的值叠加到DHRx数值上，产生伪噪声和三角波，若希望产生自定义的输出时，直接配置为DAC_WaveGeneration_None即可。

(3) DAC_LFSRUnmask_TriangleAmplitude

> 本成员通过控制DAC_CR的MAMP2位设置LFSR寄存器位的数据，即当使用伪噪声或三角波输出时要叠加到DHRx的值，非噪声或三角波输出模式下， 本配置无效。使用伪噪声输出时LFSR=0xAAA，MAMP2寄存器位可以屏蔽LFSR的某些位， 这时把本结构体成员赋值为DAC_LFSRUnmask_Bit0~DAC_LFSRUnmask_Bit11_0等宏即可；使用三角波输出时，本结构体设置三角波的最大幅值， 可选择为DAC_TriangleAmplitude_1~ DAC_TriangleAmplitude_4096等宏，见图， DAC在DHRx值的基础上升，幅值达到MAMPx设置的最大幅度时下降，形成三角波的输出。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/DAC003.png)

(4) DAC_OutputBuffer

> 本结构体成员用于控制是否使能DAC的输出缓冲（DAC_OutputBuffer_Enable/Disable）， 使能了DAC的输出缓冲后可以减小输出阻抗，适合直接驱动一些外部负载。

## 4. 实例

#### 4.1 包含头文件

确保你包含了所需的头文件

```c
#include "stm32f4xx.h"  // 根据你的具体STM32型号调整
```

#### 4.2 DAC初始化函数

我们需要一个函数来初始化DAC：

```c
void DAC_Init(void) {
    // 启用DAC时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    // DAC配置
    DAC_InitTypeDef DAC_InitStruct;

    DAC_InitStruct.DAC_Trigger = DAC_Trigger_T6_TRGO; // 定时器6触发
    DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_Triangle; // 生成三角波
    DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_2047; // 三角波幅值
    DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Enable; // 启用输出缓冲器

    // 初始化DAC
    DAC_Init(DAC_Channel_1, &DAC_InitStruct);

    // 启用DAC通道
    DAC_Cmd(DAC_Channel_1, ENABLE);
}
```

#### 4.3 定时器配置

为了生成三角波，需要配置一个定时器作为DAC的触发源：

```c
void Timer_Config(void) {
    // 启用定时器6时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    // 定时器基本配置
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_InitStruct.TIM_Period = 1000; // 设置计数周期
    TIM_InitStruct.TIM_Prescaler = 84 - 1; // 设置预分频
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;

    // 初始化定时器
    TIM_TimeBaseInit(TIM6, &TIM_InitStruct);

    // 启用定时器更新中断
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    // 启动定时器
    TIM_Cmd(TIM6, ENABLE);
}
```

#### 4.4 主函数

将上述函数在主函数中调用：

```c
int main(void) {
    // 系统初始化
    SystemInit();

    // DAC和定时器初始化
    DAC_Init();
    Timer_Config();

    while (1) {
        // 主循环内容
    }
}
```

#### 4.5 处理中断

如果需要在定时器溢出时更新DAC输出，可以实现一个中断服务程序：

```c
void TIM6_DAC_IRQHandler(void) {
    // 检查更新中断标志
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
        // 清除中断标志
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

        // 更新DAC输出值（简单的例子，实际中可能需要更复杂的波形计算）
        static uint32_t triangle_value = 0;
        triangle_value = (triangle_value + 1) % 4096; // 更新值范围0-4095
        DAC_SetChannel1Data(DAC_Align_12b_R, triangle_value); // 输出到DAC通道
    }
}
```



---

2024.9.22 第一次修订，后期不再维护

2025.1.31 修补内容
