# 第四十六章 DAC-输出正弦波

## 1. 硬件设计

STM32的DAC外设有固定的输出通道，分别为PA4和PA5，不过，指南者开发板已经在板载SPI-FLASH芯片中使用了这两个引脚， 所以用作DAC通道输出电压时会受到干扰，影响实验

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/DAC004.jpeg)

PA5引脚连接到FLASH芯片的CLK引脚中，这可能会干扰DAC实验输出的电压信号，导致得不到正确的波形。经测试，在进行本实验时，只要不使用SPI-FLASH，DAC可以输出正常的波形，所以DAC和SPI-FLASH不在同一个实验中使用即可。

在设计DAC专门的实际应用时，DAC的输出通道应独占，不与其它设备共用。本开发板的设计是考虑到各种资源分配，才不得不占用DAC通道的。

实验时直接使用示波器测量PA4和PA5引脚的输出即可。

## 2. 软件设计

### 2.1 编程大纲

1) DAC配置通道

2) 定时器配置

3) DMA配置

4) 初始化DAC工作模式

5) 主函数测试

### 2.2 代码分析

#### 2.2.1 生成正弦波数据表

要输出正弦波，实质是要控制DAC以v=sin(t)的正弦函数关系输出电压，其中v为电压输出，t为时间。

而由于模拟信号连续而数字信号是离散的，所以使用DAC产生正弦波时，只能按一定时间间隔输出正弦曲线上的点， 在该时间段内输出相同的电压值，若缩短时间间隔，提高单个周期内的输出点数，可以得到逼近连续正弦波的图形， 见图，若在外部电路加上适当的电容滤波，可得到更完美的图形。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/DAC005.jpg)

由于正弦曲线是周期函数，所以只需要得到单个周期内的数据后按周期重复即可，而单个周期内取样输出的点数又是有限的， 所以为了得到呈v=sin(t)函数关系电压值的数据通常不会实时计算获取，而是预先计算好函数单个周期内的电压数据表，并且转化成以DAC寄存器表示的值。

如sin函数值的范围为[-1: +1]，而STM32的DAC输出电压范围为[0~3.3]V，按12位DAC分辨率表示的方法， 可写入寄存器的最大值为2^12 = 4096，即范围为[0:4096]。所以，实际输出时，会进行如下处理：

1) 抬升sin函数的输出为正值：v = sin(t)+1 ， 此时，v的输出范围为[0:2]；

2) 扩展输出至DAC的全电压范围: v = 3.3*(sin(t)+1)/2 ， 此时，v的输出范围为[0:3.3]，正是DAC的电压输出范围，扩展至全电压范围可以充分利用DAC的分辨率；

3) 把电压值以DAC寄存器的形式表示：Reg_val = 212/3.3 * v = 211*(sin(t)+1)， 此时，存储到DAC寄存器的值范围为[0:4096]；

4) 实践证明，在sin(t)的单个周期内，取32个点进行电压输出已经能较好地还原正弦波形， 所以在t∈[0:2π]区间内等间距根据上述Reg_val公式运算得到32个寄存器值，即可得到正弦波表；

5) 控制DAC输出时，每隔一段相同的时间从上述正弦波表中取出一个新数据进行输出， 即可输出正弦波。改变间隔时间的单位长度，可以改变正弦波曲线的周期。

为方便起见，我们使用了Python和Matlab脚本制作正弦波表，脚本的代码存储在本工程的目录下，感兴趣可以打开文件查看

Python脚本的实现原理就是前面介绍的正弦波数据表的制作过程，运行后，该脚本把得到的正弦波表数据输出到目录下的py_dac_sinWav.c文件中

```c
[2048, 2460, 2856, 3218, 3532, 3786, 3969, 4072,
 4093, 4031, 3887, 3668, 3382, 3042, 2661, 2255,
 1841, 1435, 1054,  714,  428,  209,   65,    3,
   24,  127,  310,  564,  878, 1240, 1636, 2048]
```

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/DAC006.png)

#### 2.2.2 DAC宏定义

```c
// DAC DHR12RD寄存器，12位、右对齐、双通道
#define DAC_DHR12RD_ADDRESS (DAC_BASE+0x20)
```

此处定义的宏DAC_DHR12RD_ ADDRESS是寄存器DHR12RD的地址，该寄存器是12位右对齐的双通道寄存器， 见图。在本实验中将会使用DMA把正弦波数据表的点数据赋值到该寄存器中， 往该寄存器赋值后的数据会在DAC被触发的时候搬运到2个DAC转换器，然后在这2个通道中输出以12位右对齐表示的这两个通道的电压。DAC中还有其它寄存器， 它们的功能类似，可以在《STM32中文参考手册》中了解到。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/DAC007.png)

与DAC控制相关的引脚固定是PA4和PA5，就不使用宏定义了，在源代码中会直接使用引脚号操作。

#### 2.2.3 DAC GPIO和模式配置

```c
static void DAC_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef  DAC_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // 配置DAC 通道1
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;						// 使用TIM2作为触发源
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;	// 不使用波形发生器
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;	// 不使用DAC输出缓冲
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  // 配置DAC 通道2
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);
  DAC_Cmd(DAC_Channel_1, ENABLE);
  DAC_Cmd(DAC_Channel_2, ENABLE);
  DAC_DMACmd(DAC_Channel_2, ENABLE);
}
```

在DAC_Config函数中，完成了DAC通道的GPIO的初始化和DAC模式配置 。 其中GPIO按照要求被配置为模拟输入模式（没有模拟输出模式），在该模式下才能正常输出模拟信号。

配置DAC工作模式时，则使用了DAC_InitTypeDef 类型的初始化结构体，把DAC通道1和2都配置成了使用定时器TIM2触发、 不使用波形发生器以及不使用DAC输出缓冲的模式。

初始化完GPIO和DAC模式后，还使用了DAC_Cmd、DAC_DMACmd函数使能了通道以及DMA的请求。由于本实验中对DAC1和2的操作是同步的， 所以只要把DMA与DAC通道2关联起来即可，当使用DMA设置通道2的数据值时，同时更新通道1的内容。

定时器配置及计算正弦波的频率

初始化完DAC后，需要配置触发用的定时器，设定每次触发的间隔，以达到控制正弦波周期的目的。

#### 2.2.4 TIM配置

```c
static void DAC_TIM_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;	
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

因为前面的DAC配置了TIM2当触发源，所以这里将对TIM2进行配置。TIM2的定时周期被配置为20，向上计数，不分频。 即TIM2每隔20*（1/72M）秒就会触发一次DAC事件，作DAC触发源使用的定时器并不需要设置中断，当定时器计数器向上计数至指定的值时， 产生Update事件，同时触发DAC把DHRx寄存器的数据转移到DORx，从而开始进行转换。

<img src="https://img.picgo.net/2024/09/04/-2024-09-04-160543e1c0b83bf0fefb0a.png" title="" alt="屏幕截图 2024 09 04 160543" width="728">

在实际应用中，可以根据工程里的正弦波点数和定时器配置生成特定频率的正弦波。

#### 2.2.5 DMA配置

```c
static void DAC_DMA_Config(void)
{	
	DMA_InitTypeDef  DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_ADDRESS;	 // 外设数据地址 寄存器 DHR12RD 的地址12位、右对齐、双通道
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DualSine12bit; // 内存数据地址 DualSine12bit
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;							 // 数据传输方向内存至外设
  DMA_InitStructure.DMA_BufferSize = PWM_Num;													
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

在上述代码中，定义了由脚本得到的正弦波数据表Sine12bit变量，一共为POINT_NUM（32）个点。在DAC_Mode_Init函数中， 调用了前面介绍的DAC_Config和DAC_TIM_Config初始化DAC和定时器，然后在for循环中把单通道的正弦波数据表Sine12bit复制扩展成为双通道的数据DualSine12bit， 扩展后的数据将会直接被DMA搬运至DAC的DHR12RD寄存器中。

复制完数据后，DAC_Mode_Init调用下面的DAC_DMA_Config函数初始化DMA，配置的重点是要设置好DHR12RD寄存器的地址， 正弦波数据的内存地址（注意是双通道数据DualSine12bit），DMA缓存的个数（即单个周期的正弦波点数）以及DMA工作在循环模式。

经过这样的配置后，定时器每间隔一定的时间就会触发DMA搬运双通道正弦波表的一个数据到DAC双通道寄存器进行转换， 每完成一个周期后DMA重新开始循环，从而达到连续输出波形的目的。

#### 2.2.6 主函数

```c
#include "stm32f10x.h"
#include "dac.h"

int main(void)
{
	DAC_Mode_Init();
  	while(1);	 
}

```

本工程的主函数非常简单，直接调用DAC_Mode_Init即可完成所有的配置，此时再使用示波器测量PA4、PA5引脚可查看其输出的波形。

## 3. 小结

DAC输出波形配置倒是不难，我们同时还运用了定时器和DMA传输，看起来有点复杂，实际上就是对前面几章的综合运用而已，下面我们来简单回顾一下整个流程：

### 3.1 硬件要求

- STM32微控制器（如STM32F4系列）
- 示波器或其他测量设备连接到DAC输出

### 3.2 实验设计

#### 1. 硬件连接

- 确保DAC的输出引脚连接到适当的测试设备（例如示波器）。
- 将TIM配置为适当的时钟源。

#### 2. 正弦波数据生成

我们将生成一个包含100个样本的正弦波数据数组，范围从0到4095（对于12位DAC）。

#### 3. 示例代码

```c
#include "stm32f4xx.h"
#include <math.h>

#define SINE_SAMPLES 100
uint16_t sine_wave[SINE_SAMPLES];

// 生成正弦波样本
void generate_sine_wave() {
    for (int i = 0; i < SINE_SAMPLES; i++) {
        sine_wave[i] = (uint16_t)((sin((2 * M_PI * i) / SINE_SAMPLES) + 1) * 2047.5);
    }
}

// 初始化DAC
void DAC_Init() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    DAC_InitTypeDef DAC_InitStruct;
    DAC_InitStruct.DAC_Trigger = DAC_Trigger_T6_TRGO; // 使用定时器6触发
    DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStruct);

    DAC_Cmd(DAC_Channel_1, ENABLE); // 启用DAC通道1
}

// 初始化TIM
void TIM_Init() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_InitStruct.TIM_Period = 100 - 1; // 自动重装载寄存器
    TIM_InitStruct.TIM_Prescaler = 1600 - 1; // 预分频器
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_InitStruct);

    TIM_SelectOutputTrigger(TIM6, TIM_TRIGER_UPDATE); // 选择触发方式
    TIM_Cmd(TIM6, ENABLE); // 启动TIM6
}

// 初始化DMA
void DMA_Init() {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    DMA_InitTypeDef DMA_InitStruct;
    DMA_InitStruct.DMA_Channel = DMA_Channel_7; // 选择通道
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R1; // DAC数据寄存器地址
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)sine_wave; // 源地址
    DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral; // 从内存到外设
    DMA_InitStruct.DMA_BufferSize = SINE_SAMPLES; // 数据量
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不递增
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址递增
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // 外设数据大小
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word; // 内存数据大小
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular; // 循环模式
    DMA_InitStruct.DMA_Priority = DMA_Priority_High; // 优先级
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable; // 禁用FIFO
    DMA_Init(DMA2_Stream5, &DMA_InitStruct);

    DMA_Cmd(DMA2_Stream5, ENABLE); // 启用DMA流
    DAC_DMACmd(DAC_Channel_1, ENABLE); // 启用DAC的DMA
}

// 主程序
int main(void) {
    SystemInit(); // 系统初始化
    generate_sine_wave(); // 生成正弦波数据
    DAC_Init(); // 初始化DAC
    TIM_Init(); // 初始化TIM
    DMA_Init(); // 初始化DMA

    while (1) {
        // 主循环，DAC和DMA在后台工作
    }
}
```

---

2024.9.23 第一次修订，后期不再维护

2025.1.31 修补内容
