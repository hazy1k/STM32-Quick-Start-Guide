# 第二十三章 STM32F4 DAC简介

## 1. STM32F4 DAC 简介

STM32F4 的 DAC 模块(数字/模拟转换模块)是 12 位数字输入，电压输出型的 DAC。DAC可以配置为 8 位或 12 位模式，也可以与 DMA 控制器配合使用。 DAC 工作在 12 位模式时，数据可以设置成左对齐或右对齐。 DAC 模块有 2 个输出通道，每个通道都有单独的转换器。在双 DAC 模式下， 2 个通道可以独立地进行转换，也可以同时进行转换并同步地更新 2 个通道的输出。 DAC 可以通过引脚输入参考电压 Vref+（通 ADC 共用） 以获得更精确的转换结果。

STM32F4 的 DAC 模块主要特点有：

- 2 个 DAC 转换器：每个转换器对应 1 个输出通道

- 8 位或者 12 位单调输出

- 12 位模式下数据左对齐或者右对齐

- 同步更新功能

- 噪声波形生成

- 三角波形生成

- 双 DAC 通道同时或者分别转换

- 每个通道都有 DMA 功能

## 2. 单个DAC通道的框图

单个 DAC 通道的框图如图所示：

![屏幕截图 2024-10-15 184128.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/15-18-42-10-屏幕截图%202024-10-15%20184128.png)

图中 VDDA 和 VSSA 为 DAC 模块模拟部分的供电，而 Vref+则是 DAC 模块的参考电压。DAC_OUTx 就是 DAC 的输出通道了（对应 PA4 或者 PA5 引脚）。

DAC 输出是受 DORx 寄存器直接控制的，但是我们不能直接往DORx 寄存器写入数据，而是通过 DHRx 间接的传给 DORx 寄存器，实现对 DAC 输出的控制。前面我们提到， STM32F4 的 DAC 支持 8/12 位模式， 8 位模式的时候是固定的右对齐的，而 12 位模式又可以设置左对齐/右对齐。单 DAC 通道 x，总共有 3 种情况：

1. 8位数据右对齐：用户将数据写入DAC_DHR8Rx[7:0]位（实际存入DHRx[11:4]位）。

2. 12 位数据左对齐：用户将数据写入 DAC_DHR12Lx[15:4]位（实际存入 DHRx[11:0]位）。

3. 12 位数据右对齐：用户将数据写入 DAC_DHR12Rx[11:0]位（实际存入 DHRx[11:0]位）。

我们本章使用的就是单 DAC 通道 1，采用 12 位右对齐格式，所以采用第3种情况。

如果没有选中硬件触发(寄存器 DAC_CR1 的 TENx 位置’ 0’ )，存入寄存器 DAC_DHRx的数据会在一个 APB1 时钟周期后自动传至寄存器 DAC_DORx。如果选中硬件触发(寄存器DAC_CR1 的 TENx 位置’ 1’ )，数据传输在触发发生以后 3 个 APB1 时钟周期后完成。

一旦数据从 DAC_DHRx 寄存器装入 DAC_DORx 寄存器，在经过时间 <img title="" src="https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/15-18-48-39-屏幕截图%202024-10-15%20184757.png" alt="屏幕截图 2024-10-15 184757.png" width="80">之后，输出即有效，这段时间的长短依电源电压和模拟输出负载的不同会有所变化。我们可以从STM32F407ZGT6 的数据手册查到<img src="https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/15-18-48-39-屏幕截图%202024-10-15%20184757.png" title="" alt="屏幕截图 2024-10-15 184757.png" width="82">的典型值为 3us，最大是 6us。所以 DAC 的转换速度最快是 333K 左右。

本章我们将不使用硬件触发（TEN=0），其转换的时间框图如图：

![屏幕截图 2024-10-15 184958.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/15-18-50-05-屏幕截图%202024-10-15%20184958.png)

当 DAC 的参考电压为 Vref+的时候， DAC 的输出电压是线性的从 0~Vref+， 12 位模式下 DAC 输出电压与 Vref+以及 DORx 的计算公式如下：

![屏幕截图 2024-10-15 185023.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/15-18-50-27-屏幕截图%202024-10-15%20185023.png)

## 3. 相关寄存器介绍

### 3.1 控制寄存器（DAC_CR）

![屏幕截图 2024-10-16 084732.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/16-08-47-42-屏幕截图%202024-10-16%20084732.png)

DAC_CR 的低 16 位用于控制通道 1，而高 16 位用于控制通道 2，我们这里仅列出比较重要的最低 8 位的详细描述，如图：

![屏幕截图 2024-10-16 084814.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/16-08-48-17-屏幕截图%202024-10-16%20084814.png)

首先，我们来看 DAC 通道 1 使能位(EN1)，该位用来控制 DAC 通道 1 使能的，本章我们就是用的 DAC 通道 1，所以该位设置为 1。

再看关闭 DAC 通道 1 输出缓存控制位（BOFF1），这里 STM32F4 的 DAC 输出缓存做的有些不好，如果使能的话，虽然输出能力强一点，但是输出没法到 0，这是个很严重的问题。所以本章我们不使用输出缓存。即设置该位为 1。

DAC 通道 1 触发使能位（TEN1），该位用来控制是否使用触发，里我们不使用触发，所以设置该位为 0。

DAC 通道 1 触发选择位（TSEL1[2:0]），这里我们没用到外部触发，所以设置这几个位为 0 就行了。

DAC 通道 1 噪声/三角波生成使能位（WAVE1[1:0]），这里我们同样没用到波形发生器，故也设置为 0 即可。

DAC 通道 1 屏蔽/复制选择器（MAMP[3:0]），这些位仅在使用了波形发生器的时候有用，本章没有用到波形发生器，故设置为 0 就可以了。

最后是 DAC 通道 1 DMA 使能位（DMAEN1），本章我们没有用到 DMA 功能，故还是设置为 0。

### 3.2 12位右对齐数据保存寄存器（DAC_DHR12R1）

![屏幕截图 2024-10-16 085212.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/16-08-52-18-屏幕截图%202024-10-16%20085212.png)

该寄存器用来设置 DAC 输出，通过写入 12 位数据到该寄存器，就可以在 DAC 输出通道 1（PA4）得到我们所要的结果。

通过以上介绍，我们了解了 STM32F4 实现 DAC 输出的相关设置，本章我们将使用 DAC模块的通道 1 来输出模拟电压。

## 4. DAC配置基本步骤

### 4.1 开启PA口时钟，设置PA4为模拟输入

STM32F407ZGT6 的 DAC 通道 1 是接在 PA4 上的，所以，我们先要使能 GPIOA 的时钟，然后设置 PA4 为模拟输入。

这里需要特别说明一下，虽然 DAC 引脚设置为输入，但是 STM32F4 内部会连接在 DAC模拟输出上,这在我们引脚复用映射章节有讲解。程序如下：

```c
__HAL_RCC_DAC_CLK_ENABLE();   // 使能 DAC 时钟
__HAL_RCC_GPIOA_CLK_ENABLE(); // 开启 GPIOA 时钟
GPIO_Initure.Pin=GPIO_PIN_4;  // PA4
GPIO_Initure.Mode=GPIO_MODE_ANALOG; // 模拟
GPIO_Initure.Pull=GPIO_NOPULL;// 不带上下拉
HAL_GPIO_Init(GPIOA,&GPIO_Initure);
```

对于 DAC 通道与引脚对应关系，这在 STM32F4 的数据手册引脚表上有列出，如下图：

![屏幕截图 2024-10-16 085456.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/16-08-55-01-屏幕截图%202024-10-16%20085456.png)

### 4.2 初始化DAC，设置DAC的工作模式

HAL 库中提供了一个 DAC 初始化函数 HAL_DAC_Init，该函数声明如下：

```c
HAL_StatusTypeDef HAL_DAC_Init(DAC_HandleTypeDef* hdac);
```

该函数并没有设置任何 DAC 相关寄存器，也就是说没有对 DAC 进行任何配置，它只是 HAL 库提供用来在软件上初始化 DAC，也就是说，为后面 HAL 库操作 DAC 做好准备。它有一个很重要的作用就是在函数内部会调用 DAC 的 MSP 初始化函数 HAL_DAC_MspInit，该函数声明如下：

```c
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac);
```

一般情况下，步骤 1 中的与 MCU 相关的时钟使能和 IO 口配置都放在该函数中实现。

HAL 库提供了一个很重要的 DAC 配置函数 HAL_DAC_ConfigChannel，该函数用来配置 DAC 通道的触发类型以及输出缓冲。该函数声明如下：

```c
HAL_StatusTypeDef HAL_DAC_ConfigChannel(DAC_HandleTypeDef* hdac,
                                        DAC_ChannelConfTypeDef* sConfig, uint32_t Channel);
```

第一个入口参数非常简单，为 DAC 初始化句柄， 和 HAL_DAC_Init 保存一致即可。

第三个入口参数 Channel 用来配置 DAC 通道，比如我们使用 PA4，也就是 DAC 通道 1，所以配置值为 DAC_CHANNEL_1 即可。

接下来我们看看第二个入口参数 sConfig，该参数是 DAC_ChannelConfTypeDef 结构体指针类型，结构体 DAC_ChannelConfTypeDef 定义如下：

```c
typedef struct
{
    uint32_t DAC_Trigger; // DAC 触发类型
    uint32_t DAC_OutputBuffer; // 输出缓冲
}DAC_ChannelConfTypeDef;
```

成员变量DAC_Trigger 用来设置DAC 触发类型， DAC_OutputBuffer 用来设置输出缓冲，这在我们前面都有讲解。 DAC 初始化配置实例代码如下：

```c
DAC_HandleTypeDef DAC1_Handler;
DAC_ChannelConfTypeDef DACCH1_Config;
DAC1_Handler.Instance=DAC;
HAL_DAC_Init(&DAC1_Handler); //初始化 DAC
DACCH1_Config.DAC_Trigger=DAC_TRIGGER_NONE; //不使用触发功能
DACCH1_Config.DAC_OutputBuffer=DAC_OUTPUTBUFFER_DISABLE;
HAL_DAC_ConfigChannel(&DAC1_Handler,&DACCH1_Config,DAC_CHANNEL_1);
```

### 4.3 使能DAC转换通道

初始化 DAC 之后，理所当然要使能 DAC 转换通道， HAL 库函数是：

```c
HAL_StatusTypeDef HAL_DAC_Start(DAC_HandleTypeDef* hdac, uint32_t Channel);
```

该函数非常简单，第一个参数是 DAC 句柄，第二个用来设置 DAC 通道。

### 4.4 设置DAC的输出值

通过前面 3 个步骤的设置， DAC 就可以开始工作了，我们使用 12 位右对齐数据格式，，就可以在 DAC 输出引脚（PA4）得到不同的电压值了， HAL 库函数为：

```c
HAL_StatusTypeDef HAL_DAC_SetValue(DAC_HandleTypeDef* hdac,
                                   uint32_t Channel, uint32_t Alignment, uint32_t Data);
```

该函数从入口参数可以看出，它是配置 DAC 的通道输出值，同时通过第三个入口参数设置对齐方式。

通过以上几个步骤的设置，我们就能正常的使用 STM32F4 的 DAC 通道 1 来输出不同的模拟电压了。
