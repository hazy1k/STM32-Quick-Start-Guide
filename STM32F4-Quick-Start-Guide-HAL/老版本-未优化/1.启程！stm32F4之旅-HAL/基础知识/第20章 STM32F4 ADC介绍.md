# 第二十章 STM32F4 ADC介绍

## 1. STM32F4 ADC简介

STM32F4xx 系列一般都有 3 个 ADC，这些 ADC 可以独立使用，也可以使用双重/三重模式（提高采样率）。 STM32F4 的 ADC 是 12 位逐次逼近型的模拟数字转换器。它有 19 个通道，可测量 16 个外部源、 2 个内部源和 Vbat 通道的信号。 这些通道的 A/D 转换可以单次、连续、扫描或间断模式执行。 ADC 的结果可以左对齐或右对齐方式存储在 16 位数据寄存器中。 模拟看门狗特性允许应用程序检测输入电压是否超出用户定义的高/低阀值。

STM32F407ZGT6 包含有 3 个 ADC。 STM32F4 的 ADC 最大的转换速率为 2.4Mhz，也就是转换时间为 0.41us（在 ADCCLK=36M,采样周期为 3 个 ADC 时钟下得到），不要让 ADC 的时钟超过 36M，否则将导致结果准确度下降。

STM32F4 将 ADC 的转换分为 2 个通道组：规则通道组和注入通道组。规则通道相当于你正常运行的程序，而注入通道呢，就相当于中断。在你程序正常执行的时候，中断是可以打断你的执行的。同这个类似，注入通道的转换可以打断规则通道的转换， 在注入通道被转换完成之后，规则通道才得以继续转换。

通过一个形象的例子可以说明： 假如你在家里的院子内放了 5 个温度探头，室内放了 3 个温度探头； 你需要时刻监视室外温度即可，但偶尔你想看看室内的温度；因此你可以使用规则通道组循环扫描室外的 5 个探头并显示 AD 转换结果，当你想看室内温度时，通过一个按钮启动注入转换组(3 个室内探头)并暂时显示室内温度，当你放开这个按钮后，系统又会回到规则通道组继续检测室外温度。从系统设计上，测量并显示室内温度的过程中断了测量并显示室外温度的过程，但程序设计上可以在初始化阶段分别设置好不同的转换组，系统运行中不必再变更循环转换的配置，从而达到两个任务互不干扰和快速切换的结果。可以设想一下，如果没有规则组和注入组的划分，当你按下按钮后，需要从新配置 AD 循环扫描的通道，然后在释放按钮后需再次配置 AD 循环扫描的通道。

上面的例子因为速度较慢，不能完全体现这样区分(规则通道组和注入通道组)的好处，但在工业应用领域中有很多检测和监视探头需要较快地处理，这样对 AD 转换的分组将简化事件处理的程序并提高事件处理的速度。

STM32F4 其 ADC 的规则通道组最多包含 16 个转换，而注入通道组最多包含 4 个通道。

---

### 1. **ADC 概述**

ADC（Analog-to-Digital Converter，模拟到数字转换器）是将连续的模拟信号转换为离散数字信号的设备。在 STM32F4 系列中，ADC 通常用于读取传感器信号、监测电压和电流等应用。

### 2. **主要特点**

- **分辨率**：STM32F4 系列的 ADC 通常提供 12 位、10 位、8 位和 6 位的分辨率，12 位的 ADC 分辨率意味着可以提供 4096 个不同的数字值。

- **采样率**：支持高达 2.4 MSPS（百万次采样每秒）的采样速率，适合高频信号的处理。

- **输入通道**：STM32F4 系列的 ADC 可以支持多通道输入，通常可以达到 16 个或更多通道（取决于具体型号）。

- **内置采样保持电路**：在转换过程中可以保持输入电压，以提高转换精度。

- **内置参考电压**：支持使用内置参考电压（如 VREF+ 和 VREF-），也可以使用外部参考电压。

- **工作模式**：支持普通模式和低功耗模式，能够根据应用需求进行灵活配置。

- **DMA 支持**：ADC 可以与 DMA（Direct Memory Access，直接存储器访问）配合使用，允许在不占用 CPU 的情况下自动将转换结果传输到内存中。

- **转换触发**：支持多种触发源（如定时器、外部中断等）来启动 ADC 转换。

### 3. **ADC 工作原理**

ADC 的工作流程一般包括以下步骤：

1. **选择输入通道**：通过配置 ADC 寄存器选择需要测量的输入通道。

2. **启动转换**：可以通过软件启动转换或使用外部触发信号启动。

3. **采样保持**：在转换过程中，ADC 将输入信号的电压值保持一定时间，以确保准确的转换。

4. **数字转换**：ADC 将模拟信号转换为相应的数字值。

5. **读取结果**：转换完成后，读取转换结果并进行后续处理。

## 2. ADC相关寄存器

我们介绍一下我们执行规则通道的单次转换，需要用到的 ADC 寄存器。

### 2.1 控制寄存器（ADC_CR1和ADC_CR2）

![屏幕截图 2024-10-12 171222.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/12-17-12-44-屏幕截图%202024-10-12%20171222.png)

ADC_CR1 的 SCAN 位，该位用于设置扫描模式，由软件设置和清除，如果设置为 1，则使用扫描模式，如果为 0，则关闭扫描模式。在扫描模式下，由 ADC_SQRx 或 ADC_JSQRx 寄存器选中的通道被转换。如果设置了 EOCIE 或 JEOCIE，只在最后一个通道转换完毕后才会产生 EOC 或 JEOC 中断。

ADC_CR1[25:24]用于设置 ADC 的分辨率，详细的对应关系如图：

![屏幕截图 2024-10-12 171437.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/12-17-14-48-屏幕截图%202024-10-12%20171437.png)

本章我们使用 12 位分辨率，所以设置这两个位为 0 就可以了。接着我们介绍 ADC_CR2，该寄存器的各位描述如图：

![屏幕截图 2024-10-12 171511.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/12-17-15-18-屏幕截图%202024-10-12%20171511.png)

该寄存器我们也只针对性的介绍一些位： ADON 位用于开关 AD 转换器。而 CONT 位用于设置是否进行连续转换，我们使用单次转换，所以 CONT 位必须为 0。 ALIGN 用于设置数据对齐，我们使用右对齐，该位设置为 0。

EXTEN[1:0]用于规则通道的外部触发使能设置， 详细的设置关系如图：

![屏幕截图 2024-10-12 171643.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/12-17-16-58-屏幕截图%202024-10-12%20171643.png)

我们这里使用的是软件触发，即不使用外部触发， 所以设置这 2 个位为 0 即可。 ADC_CR2的 SWSTART 位用于开始规则通道的转换，我们每次转换（单次转换模式下）都需要向该位写1。

### 2.2 通用控制寄存器（ADC_CCR）

![屏幕截图 2024-10-13 090816.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/13-09-08-25-屏幕截图%202024-10-13%20090816.png)

该寄存器我们也只针对性的介绍一些位： TSVREFE 位是内部温度传感器和 Vrefint 通道使能位，内部温度传感器我们将在下一章介绍，这里我们直接设置为 0。 ADCPRE[1:0]用于设置ADC 输入时钟分频， 00~11 分别对应 2/4/6/8 分频， STM32F4 的 ADC 最大工作频率是 36Mhz，而 ADC时钟（ADCCLK）来自 APB2， APB2频率一般是 84Mhz，所以我们一般设置 ADCPRE=01，即 4 分频，这样得到 ADCCLK 频率为 21Mhz。 MULTI[4:0]用于多重 ADC 模式选择， 详细的设置关系如图：

![屏幕截图 2024-10-13 090939.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/13-09-09-43-屏幕截图%202024-10-13%20090939.png)

本章我们仅用了 ADC1（独立模式），并没用到多重 ADC 模式，所以设置这 5 个位为 0 即可。

### 2.3 采样时间寄存器（ADC_SMPR1和ADC_SMPR2）

这两个寄存器用于设置通道 0~18 的采样时间，每个通道占用 3 个位。 ADC_SMPR1 的各位描述如图：

![屏幕截图 2024-10-13 091052.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/13-09-10-56-屏幕截图%202024-10-13%20091052.png)

ADC_SMPR2 的各位描述如下图所示：

![屏幕截图 2024-10-13 091136.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/13-09-11-39-屏幕截图%202024-10-13%20091136.png)

对于每个要转换的通道，采样时间建议尽量长一点，以获得较高的准确度， 但是这样会降低 ADC 的转换速率。 ADC 的转换时间可以由以下公式计算：

                                                    Tcovn=采样时间+12 个周期

其中： Tcovn 为总转换时间，采样时间是根据每个通道的 SMP 位的设置来决定的。例如，当 ADCCLK=21Mhz 的时候，并设置 3 个周期的采样时间，则得到： Tcovn=3+12=15 个周期=0.71us。

### 2.4 规则序列寄存器（ADC_SQR1~3）

该寄存器总共有 3 个，这几个寄存器的功能都差不多，这里我们仅介绍一下 ADC_SQR1，该寄存器的各位描述如图：

![屏幕截图 2024-10-13 091342.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/13-09-13-45-屏幕截图%202024-10-13%20091342.png)

L[3： 0]用于存储规则序列的长度，我们这里只用了 1 个，所以设置这几个位的值为 0。其他的 SQ13~16 则存储了规则序列中第 13~16 个通道的编号（0~18）。另外两个规则序列寄存器同 ADC_SQR1 大同小异，我们这里就不再介绍了，要说明一点的是：我们选择的是单次转换，所以只有一个通道在规则序列里面，这个序列就是 SQ1，至于 SQ1 里面哪个通道，完全由用户自己设置，通过 ADC_SQR3 的最低 5 位（也就是 SQ1） 设置。

### 2.5 规则数据寄存器（ADC_DR）

规则序列中的 AD 转化结果都将被存在这个寄存器里面，而注入通道的转换结果被保存在 ADC_JDRx 里面。 ADC_DR 的各位描述如图：

![屏幕截图 2024-10-13 091538.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/13-09-15-40-屏幕截图%202024-10-13%20091538.png)

这里要提醒一点的是，该寄存器的数据可以通过 ADC_CR2 的 ALIGN 位设置左对齐还是右对齐。在读取数据的时候要注意。

### 2.6 状态寄存器（ADC_SR）

该寄存器保存了 ADC 转换时的各种状态。

![屏幕截图 2024-10-13 091623.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/13-09-16-26-屏幕截图%202024-10-13%20091623.png)

这里我们仅介绍将要用到的是 EOC 位，我们通过判断该位来决定是否此次规则通道的 AD转换已经完成， 如果该位位 1，则表示转换完成了， 就可以从 ADC_DR 中读取转换结果，否则等待转换完成。

## 3. ADC基本配置步骤

### 3.1 开启PA口时钟和ADC1时钟，设置PA5为模拟输入

STM32F407ZGT6 的 ADC1 通道 5 在 PA5 上，所以，我们先要使能 GPIOA 的时钟，然后设置 PA5 为模拟输入。 同时我们要把 PA5 复用为 ADC，所以我们要使能 ADC1 时钟。

这里特别要提醒，对于 IO 口复用为 ADC 我们要设置模式为模拟输入，而不是复用功能，也不需要调用 GPIO_PinAFConfig 函数来设置引脚映射关系。

使能 GPIOA 时钟和 ADC1 时钟都很简单，具体方法为：

```c
__HAL_RCC_ADC1_CLK_ENABLE(); // 使能 ADC1 时钟
__HAL_RCC_GPIOA_CLK_ENABLE();// 开启 GPIOA 时钟
```

下面初始化 GPIOA5 为模拟输入

```c
GPIO_InitTypeDef GPIO_Initure;
GPIO_Initure.Pin=GPIO_PIN_5; //PA5
GPIO_Initure.Mode=GPIO_MODE_ANALOG; //模拟输入
GPIO_Initure.Pull=GPIO_NOPULL; //不带上下拉
HAL_GPIO_Init(GPIOA,&GPIO_Initure);
```

这里需要说明一下， ADC 的通道与引脚的对应关系在 STM32F4 的数据手册可以查到，我们这里使用 ADC1 的通道 5，在数据手册中的表格为：

![屏幕截图 2024-10-13 092004.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/13-09-21-56-屏幕截图%202024-10-13%20092004.png)

### 3.2 初始化 ADC，设置 ADC 时钟分频系数，分辨率，模式，扫描方式，对齐方式等信息

在 HAL 库中， 初始化 ADC 是通过函数 HAL_ADC_Init 来实现的，该函数声明为：

```c
HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef* hadc);
```

该函数只有一个入口参数 hadc，为 ADC_HandleTypeDef 结构体指针类型，结构体定义为：

```c
typedef struct
{
    ADC_TypeDef *Instance; // ADC1/ ADC2/ ADC3
    ADC_InitTypeDef Init;  // 初始化结构体变量
    __IO uint32_t NbrOfCurrentConversionRank; // 当前转换序列
    DMA_HandleTypeDef *DMA_Handle; // DMA 方式使用
    HAL_LockTypeDef Lock;
    __IO HAL_ADC_StateTypeDef State;
    __IO uint32_t ErrorCode;
}ADC_HandleTypeDef;
```

该结构体定义和其他外设比较类似，我们着重看第二个成员变量 Init 含义，它是结构体ADC_InitTypeDef 类型，结构体 ADC_InitTypeDef 定义为：

```c
typedef struct
{
    uint32_t ClockPrescaler;//分频系数 2/4/6/8 分频 ADC_CLOCK_SYNC_PCLK_DIV4
    uint32_t Resolution; //分辨率 12/10/8/6 位： ADC_RESOLUTION_12B
    uint32_t DataAlign; //对齐方式：左对齐还是右对齐： ADC_DATAALIGN_RIGHT
    uint32_t ScanConvMode; //扫描模式 DISABLE
    uint32_t EOCSelection; //EOC 标志是否设置 DISABLE
    uint32_t ContinuousConvMode;//开启连续转换模式或者单次转换模式 DISABLE
    uint32_t DMAContinuousRequests;//开启 DMA 请求连续模式或者单独模式 DISABLE
    uint32_t NbrOfConversion; //规则序列中有多少个转换 1
    uint32_t DiscontinuousConvMode;//不连续采样模式 DISABLE
    uint32_t NbrOfDiscConversion;//不连续采样通道数 0
    uint32_t ExternalTrigConv; //外部触发方式 ADC_SOFTWARE_START
    uint32_t ExternalTrigConvEdge;//外部触发边沿
}ADC_InitTypeDef;
```

这里我们需要说明一下，和其他外设一样， HAL 库同样提供了 ADC 的 MSP 初始化函数，一般情况下，时钟使能和 GPIO 初始化都会放在 MSP 初始化函数中。函数声明为：

```c
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc);
```

### 3.3 开启 AD 转换器

在设置完了以上信息后，我们就开启 AD 转换器了（通过 ADC_CR2 寄存器控制）。

```c
HAL_ADC_Start(&ADC1_Handler); // 开启 ADC
```

### 3.4 配置通道， 读取通道 ADC 值

在上面的步骤完成后， ADC 就算准备好了。接下来我们要做的就是设置规则序列 1 里面的通道，然后启动 ADC 转换。在转换结束后，读取转换结果值值就是了。

设置规则序列通道以及采样周期的函数是:

```c
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef* hadc,
                                        ADC_ChannelConfTypeDef* sConfig);
```

该函数有两个入口参数，第一个就不用多说了，接下来我们看第二个入口参数 sConfig，它是 ADC_ChannelConfTypeDef 结构体指针类型，结构体定义如下：

```c
typedef struct
{
    uint32_t Channel; // ADC 通道
    uint32_t Rank; // 规则通道中的第几个转换
    uint32_t SamplingTime; // 采样时间
    uint32_t Offset; // 备用，暂未用到
}ADC_ChannelConfTypeDef;
```

该结构体有四个成员变量，对于 STM32F4 只用到前面三个。 Channel 用来设置 ADC 通道， Rank 用来设置要配置的通道是规则序列中的第几个转换， SamplingTime 用来设置采样时间。使用实例为：

```c
ADC1_ChanConf.Channel= ADC_CHANNEL_5; // 通道 5
ADC1_ChanConf.Rank=1; // 第 1 个序列，序列 1
ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES; // 采样时间
ADC1_ChanConf.Offset=0;
HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf); // 通道配置
```

配置好通道并且使能 ADC 后，接下来就是读取 ADC 值。这里我们采取的是查询方式读取，所以我们还要等待上一次转换结束。次过程HAL库提供了专用函数HAL_ADC_PollForConversion，函数定义为：

```c
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef* hadc,
                                            uint32_t Timeout);
```

等待上一次转换结束之后，接下来就是读取 ADC 值，函数为：

```c
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef* hadc);
```
