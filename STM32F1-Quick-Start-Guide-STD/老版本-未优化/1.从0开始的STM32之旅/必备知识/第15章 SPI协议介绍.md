# 第十五章 SPI协议介绍

## 1. SPI协议简介

SPI协议是由摩托罗拉公司提出的通讯协议(Serial Peripheral Interface)，即串行外围设备接口， 是一种高速全双工的通信总线。它被广泛地使用在ADC、LCD等设备与MCU间，要求通讯速率较高的场合。

### 1.1 SPI物理层

SPI通讯设备之间的常用连接方式见图。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI002.png)

SPI通讯使用3条总线及片选线，3条总线分别为SCK、MOSI、MISO，片选线为SS，它们的作用介绍如下：

1. SS ( Slave Select)：从设备选择信号线，常称为片选信号线，也称为NSS、CS，以下用NSS表示。当有多个SPI从设备与SPI主机相连时， 设备的其它信号线SCK、MOSI及MISO同时并联到相同的SPI总线上，即无论有多少个从设备，都共同只使用这3条总线； 而每个从设备都有独立的这一条NSS信号线，本信号线独占主机的一个引脚，即有多少个从设备，就有多少条片选信号线。 I2C协议中通过设备地址来寻址、选中总线上的某个设备并与其进行通讯；而SPI协议中没有设备地址，它使用NSS信号线来寻址， 当主机要选择从设备时，把该从设备的NSS信号线设置为低电平，该从设备即被选中，即片选有效， 接着主机开始与被选中的从设备进行SPI通讯。所以SPI通讯以NSS线置低电平为开始信号，以NSS线被拉高作为结束信号。

2. SCK (Serial Clock)：时钟信号线，用于通讯数据同步。它由通讯主机产生，决定了通讯的速率，不同的设备支持的最高时钟频率不一样， 如STM32的SPI时钟频率最大为fpclk/2，两个设备之间通讯时，通讯速率受限于低速设备。

3. MOSI (Master Output， Slave Input)：主设备输出/从设备输入引脚。主机的数据从这条信号线输出， 从机由这条信号线读入主机发送的数据，即这条线上数据的方向为主机到从机。

4. MISO (Master Input,，Slave Output)：主设备输入/从设备输出引脚。主机从这条信号线读入数据， 从机的数据由这条信号线输出到主机，即在这条线上数据的方向为从机到主机。

### 1.2 协议层

与I2C的类似，SPI协议定义了通讯的起始和停止信号、数据有效性、时钟同步等环节。

#### 1.2.1 SPI基础通讯过程

先看看SPI通讯的通讯时序，见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI003.jpg)

这是一个主机的通讯时序。NSS、SCK、MOSI信号都由主机控制产生，而MISO的信号由从机产生，主机通过该信号线读取从机的数据。 MOSI与MISO的信号只在NSS为低电平的时候才有效，在SCK的每个时钟周期MOSI和MISO传输一位数据。

以上通讯流程中包含的各个信号分解如下：

#### 1.2.2 通讯的起始和停止信号

在图中的标号处，NSS信号线由高变低，是SPI通讯的起始信号。NSS是每个从机各自独占的信号线， 当从机在自己的NSS线检测到起始信号后，就知道自己被主机选中了，开始准备与主机通讯。在图中的标号处，NSS信号由低变高， 是SPI通讯的停止信号，表示本次通讯结束，从机的选中状态被取消。

#### 1.2.3 数据有效性

SPI使用MOSI及MISO信号线来传输数据，使用SCK信号线进行数据同步。MOSI及MISO数据线在SCK的每个时钟周期传输一位数据， 且数据输入输出是同时进行的。数据传输时，MSB先行或LSB先行并没有作硬性规定，但要保证两个SPI通讯设备之间使用同样的协定， 一般都会采用图中的MSB先行模式。

观察图中的标号处，MOSI及MISO的数据在SCK的上升沿期间变化输出，在SCK的下降沿时被采样。即在SCK的下降沿时刻， MOSI及MISO的数据有效，高电平时表示数据“1”，为低电平时表示数据“0”。在其它时刻，数据无效，MOSI及MISO为下一次表示数据做准备。

SPI每次数据传输可以8位或16位为单位，每次传输的单位数不受限制。

#### 1.2.4 CPOL/CPHA及通讯模式

上面讲述的图中的时序只是SPI中的其中一种通讯模式，SPI一共有四种通讯模式， 它们的主要区别是总线空闲时SCK的时钟状态以及数据采样时刻。为方便说明，在此引入“时钟极性CPOL”和“时钟相位CPHA”的概念。

时钟极性CPOL是指SPI通讯设备处于空闲状态时，SCK信号线的电平信号(即SPI通讯开始前、 NSS线为高电平时SCK的状态)。CPOL=0时， SCK在空闲状态时为低电平，CPOL=1时，则相反。

时钟相位CPHA是指数据的采样的时刻，当CPHA=0时，MOSI或MISO数据线上的信号将会在SCK时钟线的“奇数边沿”被采样。当CPHA=1时， 数据线在SCK的“偶数边沿”采样。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI004.jpeg)

我们来分析这个CPHA=0的时序图。首先，根据SCK在空闲状态时的电平，分为两种情况。 SCK信号线在空闲状态为低电平时，CPOL=0；空闲状态为高电平时，CPOL=1。

无论CPOL=0还是=1，因为我们配置的时钟相位CPHA=0，在图中可以看到，采样时刻都是在SCK的奇数边沿。 注意当CPOL=0的时候，时钟的奇数边沿是上升沿，而CPOL=1的时候，时钟的奇数边沿是下降沿。所以SPI的采样时刻不是由上升/下降沿决定的。 MOSI和MISO数据线的有效信号在SCK的奇数边沿保持不变，数据信号将在SCK奇数边沿时被采样，在非采样时刻，MOSI和MISO的有效信号才发生切换。

类似地，当CPHA=1时，不受CPOL的影响，数据信号在SCK的偶数边沿被采样，见图 CPHA=1时的SPI通讯模式_ 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI005.jpeg)

由CPOL及CPHA的不同状态，SPI分成了四种模式，见表， 主机与从机需要工作在相同的模式下才可以正常通讯，实际中采用较多的是“模式0”与“模式3”。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI01.png)

## 2. STM32的SPI特性及架构

与I2C外设一样，STM32芯片也集成了专门用于SPI协议通讯的外设。

### 2.1 STM32的SPI外设简介

STM32的SPI外设可用作通讯的主机及从机， 支持最高的SCK时钟频率为fpclk/2 (STM32F103型号的芯片默认fpclk1为36MHz， fpclk2为72MHz)，完全支持SPI协议的4种模式，数据帧长度可设置为8位或16位， 可设置数据MSB先行或LSB先行。它还支持双线全双工(前面小节说明的都是这种模式)、双线单向以及单线模式。 其中双线单向模式可以同时使用MOSI及MISO数据线向一个方向传输数据，可以加快一倍的传输速度。而单线模式则可以减少硬件接线， 当然这样速率会受到影响。我们只讲解双线全双工模式。

### 2.2 STM32的SPI架构剖析

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI006.png)

#### 2.2.1 通讯引脚

SPI的所有硬件架构都从图 中左侧MOSI、MISO、SCK及NSS线展开的。STM32芯片有多个SPI外设， 它们的SPI通讯信号引出到不同的GPIO引脚上，使用时必须配置到这些指定的引脚

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI02.png)

其中SPI1是APB2上的设备，最高通信速率达36Mbtis/s，SPI2、SPI3是APB1上的设备，最高通信速率为18Mbits/s。除了通讯速率， 在其它功能上没有差异。其中SPI3用到了下载接口的引脚，这几个引脚默认功能是下载，第二功能才是IO口，如果想使用SPI3接口， 则程序上必须先禁用掉这几个IO口的下载功能。一般在资源不是十分紧张的情况下，这几个IO口是专门用于下载和调试程序，不会复用为SPI3。

#### 2.2.2 时钟控制逻辑

SCK线的时钟信号，由波特率发生器根据“控制寄存器CR1”中的BR[0:2]位控制，该位是对fpclk时钟的分频因子， 对fpclk的分频结果就是SCK引脚的输出时钟频率，计算方法见表。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI03.png)

其中的fpclk频率是指SPI所在的APB总线频率， APB1为fpclk1，APB2为fpckl2。

通过配置“控制寄存器CR”的“CPOL位”及“CPHA”位可以把SPI设置成前面分析的4种SPI模式。

#### 2.2.3 数据控制逻辑

SPI的MOSI及MISO都连接到数据移位寄存器上，数据移位寄存器的数据来源及目标接收、发送缓冲区以及MISO、MOSI线。 当向外发送数据的时候，数据移位寄存器以“发送缓冲区”为数据源，把数据一位一位地通过数据线发送出去；当从外部接收数据的时候， 数据移位寄存器把数据线采样到的数据一位一位地存储到“接收缓冲区”中。通过写SPI的“数据寄存器DR”把数据填充到发送缓冲区中， 通讯读“数据寄存器DR”，可以获取接收缓冲区中的内容。其中数据帧长度可以通过“控制寄存器CR1”的“DFF位”配置成8位及16位模式； 配置“LSBFIRST位”可选择MSB先行还是LSB先行。

#### 2.2.4 整体控制逻辑

整体控制逻辑负责协调整个SPI外设，控制逻辑的工作模式根据我们配置的“控制寄存器(CR1/CR2)”的参数而改变， 基本的控制参数包括前面提到的SPI模式、波特率、LSB先行、主从模式、单双向模式等等。在外设工作时， 控制逻辑会根据外设的工作状态修改“状态寄存器(SR)”，我们只要读取状态寄存器相关的寄存器位， 就可以了解SPI的工作状态了。除此之外，控制逻辑还根据要求，负责控制产生SPI中断信号、DMA请求及控制NSS信号线。

实际应用中，我们一般不使用STM32 SPI外设的标准NSS信号线，而是更简单地使用普通的GPIO，软件控制它的电平输出，从而产生通讯起始和停止信号。

#### 2.2.5 通讯过程

STM32使用SPI外设通讯时，在通讯的不同阶段它会对“状态寄存器SR”的不同数据位写入参数，我们通过读取这些寄存器标志来了解通讯状态。

图中的是“主模式”流程，即STM32作为SPI通讯的主机端时的数据收发过程。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI007.jpeg)

主模式收发流程及事件说明如下：

(1) 控制NSS信号线， 产生起始信号(图中没有画出)；

(2) 把要发送的数据写入到“数据寄存器DR”中， 该数据会被存储到发送缓冲区；

(3) 通讯开始，SCK时钟开始运行。MOSI把发送缓冲区中的数据一位一位地传输出去； MISO则把数据一位一位地存储进接收缓冲区中；

(4) 当发送完一帧数据的时候，“状态寄存器SR”中的“TXE标志位”会被置1，表示传输完一帧，发送缓冲区已空；类似地， 当接收完一帧数据的时候，“RXNE标志位”会被置1，表示传输完一帧，接收缓冲区非空；

(5) 等待到“TXE标志位”为1时，若还要继续发送数据，则再次往“数据寄存器DR”写入数据即可；等待到“RXNE标志位”为1时， 通过读取“数据寄存器DR”可以获取接收缓冲区中的内容。

假如我们使能了TXE或RXNE中断，TXE或RXNE置1时会产生SPI中断信号，进入同一个中断服务函数，到SPI中断服务程序后， 可通过检查寄存器位来了解是哪一个事件，再分别进行处理。也可以使用DMA方式来收发“数据寄存器DR”中的数据。

## 3. SPI初始化结构体详解

其它外设一样，STM32标准库提供了SPI初始化结构体及初始化函数来配置SPI外设。 初始化结构体及函数定义在库文件“stm32f10x_spi.h”及“stm32f10x_spi.c”中，编程时我们可以结合这两个文件内的注释使用或参考库帮助文档。 了解初始化结构体后我们就能对SPI外设运用自如。

```c
typedef struct
{
    uint16_t SPI_Direction;         // 设置SPI的单双向模式 
    uint16_t SPI_Mode;              // 设置SPI的主/从机端模式 
    uint16_t SPI_DataSize;          // 设置SPI的数据帧长度，可选8/16
    uint16_t SPI_CPOL;              // 设置时钟极性CPOL，可选高/低电平
    uint16_t SPI_CPHA;              // 设置时钟相位，可选奇/偶数边沿采样 
    uint16_t SPI_NSS;               // 设置NSS引脚由SPI硬件控制还是软件控制
    uint16_t SPI_BaudRatePrescaler; // 设置时钟分频因子，fpclk/分频数=fSCK 
    uint16_t SPI_FirstBit;          // 设置MSB/LSB先行 
    uint16_t SPI_CRCPolynomial;     // 设置CRC校验的表达式 
} SPI_InitTypeDef;
```

这些结构体成员说明如下，其中括号内的文字是对应参数在STM32标准库中定义的宏：

1. SPI_Direction

本成员设置SPI的通讯方向，可设置为双线全双工(SPI_Direction_2Lines_FullDuplex)，双线只接收(SPI_Direction_2Lines_RxOnly)， 单线只接收(SPI_Direction_1Line_Rx)、单线只发送模式(SPI_Direction_1Line_Tx)。

2. SPI_Mode

本成员设置SPI工作在主机模式(SPI_Mode_Master)或从机模式(SPI_Mode_Slave )， 这两个模式的最大区别为SPI的SCK信号线的时序， SCK的时序是由通讯中的主机产生的。若被配置为从机模式，STM32的SPI外设将接受外来的SCK信号。

3. SPI_DataSize

本成员可以选择SPI通讯的数据帧大小是为8位(SPI_DataSize_8b)还是16位(SPI_DataSize_16b)。

4. SPI_CPOL和SPI_CPHA

这两个成员配置SPI的时钟极性CPOL和时钟相位CPHA，这两个配置影响到SPI的通讯模式， 关于CPOL和CPHA的说明参考前面“通讯模式”小节。

时钟极性CPOL成员，可设置为高电平(SPI_CPOL_High)或低电平(SPI_CPOL_Low )。

时钟相位CPHA 则可以设置为SPI_CPHA_1Edge(在SCK的奇数边沿采集数据) 或SPI_CPHA_2Edge(在SCK的偶数边沿采集数据) 。

5. SPI_NSS

本成员配置NSS引脚的使用模式，可以选择为硬件模式(SPI_NSS_Hard )与软件模式(SPI_NSS_Soft )， 在硬件模式中的SPI片选信号由SPI硬件自动产生，而软件模式则需要我们亲自把相应的GPIO端口拉高或置低产生非片选和片选信号。实际中软件模式应用比较多。

6. SPI_BaudRatePrescaler

本成员设置波特率分频因子，分频后的时钟即为SPI的SCK信号线的时钟频率。这个成员参数可设置为fpclk的2、4、6、8、16、32、64、128、256分频。

7. SPI_FirstBit

所有串行的通讯协议都会有MSB先行(高位数据在前)还是LSB先行(低位数据在前)的问题，而STM32的SPI模块可以通过这个结构体成员，对这个特性编程控制。

8. SPI_CRCPolynomial

这是SPI的CRC校验中的多项式，若我们使用CRC校验时，就使用这个成员的参数(多项式)，来计算CRC的值。

配置完这些结构体成员后，我们要调用SPI_Init函数把这些参数写入到寄存器中，实现SPI的初始化，然后调用SPI_Cmd来使能SPI外设。

---

2024.9.29 第一次修订，后期不再维护

2025.1.21 修补部分内容
