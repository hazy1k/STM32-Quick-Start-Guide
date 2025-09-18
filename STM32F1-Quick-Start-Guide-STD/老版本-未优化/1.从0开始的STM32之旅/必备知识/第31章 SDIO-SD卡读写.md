# 第三十一章 SDIO-SD卡读写

## 1. SDIO简介

SD卡(Secure Digital Memory Card)在我们生活中已经非常普遍了，控制器对SD卡进行读写通信操作一般有两种通信接口可选， 一种是SPI接口，另外一种就是SDIO接口。SDIO全称是安全数字输入/输出接口，多媒体卡(MMC)、SD卡、SD I/O卡都有SDIO接口。 STM32F10x系列控制器有一个SDIO主机接口，它可以与MMC卡、SD卡、SD I/O卡以及CE-ATA设备进行数据传输。MMC卡可以说是SD卡的前身， 现阶段已经用得很少。SD I/O卡本身不是用于存储的卡，它是指利用SDIO传输协议的一种外设。比如Wi-Fi Card， 它主要是提供Wi-Fi功能，有些Wi-Fi模块是使用串口或者SPI接口进行通信的，但Wi-Fi SDIO Card是使用SDIO接口进行通信的。 并且一般设计SD I/O卡是可以插入到SD的插槽。CE-ATA是专为轻薄笔记本硬盘设计的硬盘高速通讯接口。

随之科技发展，SD卡容量需求越来越大，SD卡发展到现在也是有几个版本的， 关于SDIO接口的设备整体概括见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SDIO002.png)

## 2. SD卡物理结构

一张SD卡包括有存储单元、存储单元接口、电源检测、卡及接口控制器和接口驱动器5个部分，见图 SD卡物理结构。 存储单元是存储数据部件，存储单元通过存储单元接口与卡控制单元进行数据传输；电源检测单元保证SD卡工作在合适的电压下， 如出现掉电或上状态时，它会使控制单元和存储单元接口复位；卡及接口控制单元控制SD卡的运行状态，它包括有8个寄存器；接口驱动器控制SD卡引脚的输入输出。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SDIO003.png)

SD卡总共有8个寄存器，用于设定或表示SD卡信息。这些寄存器只能通过对应的命令访问， 对SD卡进行控制操作并不是像操作控制器GPIO相关寄存器那样一次读写一个寄存器的，它是通过命令来控制， SDIO定义了64个命令，每个命令都有特殊意义，可以实现某一特定功能，SD卡接收到命令后， 根据命令要求对SD卡内部寄存器进行修改，程序控制中只需要发送组合命令就可以实现SD卡的控制以及读写操作。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SDIO01.png)

## 3. STM32的SDIO功能框图

STM32控制器有一个SDIO，由两部分组成：SDIO适配器和AHB接口。SDIO适配器提供SDIO主机功能， 可以提供SD时钟、发送命令和进行数据传输。AHB接口用于控制器访问SDIO适配器寄存器并且可以产生中断和DMA请求信号。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SDIO012.png)

SDIO使用两个时钟信号，一个是SDIO适配器时钟(SDIOCLK=HCLK=72MHz)，另外一个是AHB总线时钟的二分频(HCLK/2，一般为36MHz)。 适配器寄存器和FIFO使用AHB总线一侧的时钟(HCLK/2)，控制单元、命令通道和数据通道使用SDIO适配器一侧的时钟(SDIOCLK)。

SDIO_CK是SDIO接口与SD卡用于同步的时钟信号。它使用SDIOCLK作为SDIO_CK的时钟来源， 可以通过设置BYPASS模式直接得到，这时SDIO_CK = SDIOCLK=HCLK。若禁止BYPASS模式， 可以通过配置时钟寄存器的CLKDIV位控制分频因子，即SDIO_CK=SDIOCLK/（2+CLKDIV）= HCLK/（2+CLKDIV）。 配置时钟时要注意，SD卡普遍要求SDIO_CK时钟频率不能超过25MHz。

STM32控制器的SDIO是针对MMC卡和SD卡的主设备，所以预留有8根数据线，对于SD卡最多用四根数据线。

SDIO适配器是SD卡系统主机部分，是STM32控制器与SD卡数据通信中间设备。SDIO适配器由五个单元组成， 分别是控制单元、命令路径单元、数据路径单元、寄存器单元以及FIFO

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SDIO013.png)

## 4. SDIO初始化结构体讲解

标准库函数对SDIO外设建立了三个初始化结构体，分别为SDIO初始化结构体SDIO_InitTypeDef、SDIO命令初始化结构体SDIO_CmdInitTypeDef和SDIO数据初始化结构体SDIO_DataInitTypeDef。 这些结构体成员用于设置SDIO工作环境参数，并由SDIO相应初始化配置函数或功能函数调用，这些参数将会被写入到SDIO相应的寄存器，达到配置SDIO工作环境的目的。

初始化结构体和初始化库函数配合使用是标准库精髓所在，理解了初始化结构体每个成员意义基本上就可以对该外设运用自如了。 初始化结构体定义在stm32f10x_sdio.h文件中，初始化库函数定义在stm32f10x_sdio.c文件中，编程时我们可以结合这两个文件内注释使用。

SDIO初始化结构体用于配置SDIO基本工作环境，比如时钟分频、时钟沿、数据宽度等等。它被SDIO_Init函数使用。

### 4.1 SDIO初始化结构体

```c
typedef struct {
    uint32_t SDIO_ClockEdge;            // 时钟沿
    uint32_t SDIO_ClockBypass;          // 旁路时钟
    uint32_t SDIO_ClockPowerSave;       // 节能模式
    uint32_t SDIO_BusWide;              // 数据宽度
    uint32_t SDIO_HardwareFlowControl;  // 硬件流控制
    uint8_t SDIO_ClockDiv;              // 时钟分频
} SDIO_InitTypeDef;
```

各结构体成员的作用介绍如下：

1. SDIO_ClockEdge：主时钟SDIOCLK产生CLK引脚时钟有效沿选择，可选上升沿或下降沿， 它设定SDIO时钟控制寄存器(SDIO_CLKCR)的NEGEDGE位的值，一般选择设置为高电平。

2. SDIO_ClockBypass：时钟分频旁路使用，可选使能或禁用，它设定SDIO_CLKCR寄存器的BYPASS位。如果使能旁路，SDIOCLK直接驱动CLK线输出时钟； 如果禁用，使用SDIO_CLKCR寄存器的CLKDIV位值分频SDIOCLK，然后输出到CLK线。一般选择禁用时钟分频旁路

3. SDIO_ClockPowerSave：节能模式选择，可选使能或禁用，它设定SDIO_CLKCR寄存器的PWRSAV位的值。如果使能节能模式， CLK线只有在总线激活时才有时钟输出；如果禁用节能模式，始终使能CLK线输出时钟。

4. SDIO_BusWide：数据线宽度选择，可选1位数据总线、4位数据总线或8为数据总线，系统默认使用1位数据总线， 操作SD卡时在数据传输模式下一般选择4位数据总线。它设定SDIO_CLKCR寄存器的WIDBUS位的值。

5. SDIO_HardwareFlowControl：硬件流控制选择，可选使能或禁用，它设定SDIO_CLKCR寄存器的HWFC_EN位的值。 硬件流控制功能可以避免FIFO发送上溢和下溢错误。

6. SDIO_ClockDiv：时钟分频系数，它设定SDIO_CLKCR寄存器的CLKDIV位的值， 设置SDIOCLK与CLK线输出时钟分频系数：

> CLK线时钟频率=SDIOCLK/([CLKDIV+2])。

### 4.2 SDIO命令初始化结构体

SDIO命令初始化结构体用于设置命令相关内容，比如命令号、命令参数、响应类型等等。它被SDIO_SendCommand函数使用。

```c
typedef struct {
    uint32_t SDIO_Argument; // 命令参数
    uint32_t SDIO_CmdIndex; // 命令号
    uint32_t SDIO_Response; // 响应类型
    uint32_t SDIO_Wait;     // 等待使能
    uint32_t SDIO_CPSM;     // 命令路径状态机
} SDIO_CmdInitTypeDef;
```

各个结构体成员介绍如下：

1. SDIO_Argument：作为命令的一部分发送到卡的命令参数， 它设定SDIO参数寄存器(SDIO_ARG)的值。

2. SDIO_CmdIndex：命令号选择， 它设定SDIO命令寄存器(SDIO_CMD)的CMDINDEX位的值。

3. SDIO_Response：响应类型，SDIO定义两个响应类型：长响应和短响应。根据命令号选择对应的响应类型。 SDIO定义了四个32位的SDIO响应寄存器(SDIO_RESPx,x=1..4)，短响应只用到SDIO_RESP1。

4. SDIO_Wait：等待类型选择，有三种状态可选，一种是无等待状态，超时检测功能启动；一种是等待中断， 另外一种是等待传输完成。它设定SDIO_CMD寄存器的WAITPEND位和WAITINT位的值。

5. SDIO_CPSM：命令路径状态机控制，可选使能或禁用CPSM。 它设定SDIO_CMD寄存器的CPSMEN位的值。

### 4.3 SDIO数据初始化结构体

SDIO数据初始化结构体用于配置数据发送和接收参数，比如传输超时、数据长度、传输模式等等。它被SDIO_DataConfig函数使用。

```c
typedef struct {
    uint32_t SDIO_DataTimeOut;    // 数据传输超时
    uint32_t SDIO_DataLength;     // 数据长度
    uint32_t SDIO_DataBlockSize;  // 数据块大小
    uint32_t SDIO_TransferDir;    // 数据传输方向
    uint32_t SDIO_TransferMode;   // 数据传输模式
    uint32_t SDIO_DPSM;           // 数据路径状态机
} SDIO_DataInitTypeDef;
```

各结构体成员介绍如下：

1. SDIO_DataTimeOut：设置数据传输以卡总线时钟周期表示的超时周期，它设定SDIO数据定时器寄存器(SDIO_DTIMER)的值。 在DPSM进入Wait_R或繁忙状态后开始递减，直到0还处于以上两种状态则将超时状态标志置1.

2. SDIO_DataLength：设置传输数据长度， 它设定SDIO数据长度寄存器(SDIO_DLEN)的值。

3. SDIO_DataBlockSize：设置数据块大小，有多种尺寸可选， 不同命令要求的数据块可能不同。它设定SDIO数据控制寄存器(SDIO_DCTRL)寄存器的DBLOCKSIZE位的值。

4. SDIO_TransferDir：数据传输方向，可选从主机到卡的写操作， 或从卡到主机的读操作。它设定SDIO_DCTRL寄存器的DTDIR位的值。

5. SDIO_TransferMode：数据传输模式，可选数据块或数据流模式。 对于SD卡操作使用数据块类型。它设定SDIO_DCTRL寄存器的DTMODE位的值。

6. SDIO_DPSM：数据路径状态机控制，可选使能或禁用DPSM。 它设定SDIO_DCTRL寄存器的DTEN位的值。要实现数据传输都必须使能SDIO_DPSM。


