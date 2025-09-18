# 第二十七章 LCD-液晶显示介绍

## 1. 液晶控制原理

图是两种适合于STM32芯片使用的显示屏，我们以它为例讲解控制液晶屏的基本原理。

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD007.jpeg" title="" alt="" width="770">

这个完整的显示屏由液晶显示面板、电容触摸面板以及PCB底板构成。图中的触摸面板带有触摸控制芯片，该芯片处理触摸信号并通过引出的信号线与外部器件通讯， 触摸面板中间是透明的，它贴在液晶面板上面，一起构成屏幕的主体，触摸面板与液晶面板引出的排线连接到PCB底板上，根据实际需要， PCB底板上可能会带有“液晶控制器芯片”，图中右侧的液晶屏PCB上带有RA8875液晶控制器。因为控制液晶面板需要比较多的资源， 所以大部分低级微控制器都不能直接控制液晶面板，需要额外配套一个专用液晶控制器来处理显示过程，外部微控制器只要把它希望显示的数据直接交给液晶控制器即可。 而不带液晶控制器的PCB底板 ，只有小部分的电源管理电路，液晶面板的信号线与外部微控制器相连，直接控制。STM32F429系列的芯片不需要额外的液晶控制器， 也就是说它把专用液晶控制器的功能集成到STM32F429芯片内部了，可以理解为电脑的CPU集成显卡，它节约了额外的控制器成本。 而STM32F1系列的芯片由于没有集成液晶控制器到芯片内部，所以它只能驱动自带控制器的屏幕，可以理解为电脑的外置显卡。

总的来说，这两类屏幕的控制框图如图所示。

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD008.jpg" title="" alt="" width="851">

### 1.1 液晶面板的控制信号

本章我们主要讲解如何控制液晶面板，液晶面板的控制信号线即图中液晶面板引出的FPC排线， 其说明见表， 液晶面板通过这些信号线与液晶控制器通讯，使用这种通讯信号的被称为RGB接口(RGB Interface)。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD01.png)

(1) RGB信号线

> RGB信号线各有8根，分别用于表示液晶屏一个像素点的红、绿、蓝颜色分量。使用红绿蓝颜色分量来表示颜色是一种通用的做法， 打开Windows系统自带的画板调色工具，可看到颜色的红绿蓝分量值。 常见的颜色表示会在“RGB”后面附带各个颜色分量值的数据位数，如RGB565表示红绿蓝的数据线数分别为5、6、5根， 一共为16个数据位，可表示216种颜色；而这个液晶屏的种颜色分量的数据线都有8根， 所以它支持RGB888格式，一共24位数据线，可表示的颜色为224种。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD009.png)

(2) 同步时钟信号CLK

> 液晶屏与外部使用同步通讯方式，以CLK信号作为同步时钟，在同步时钟的驱动下，每个时钟传输一个像素点数据。

(3) 水平同步信号HSYNC

> 水平同步信号HSYNC(Horizontal Sync)用于表示液晶屏一行像素数据的传输结束， 每传输完成液晶屏的一行像素数据时，HSYNC会发生电平跳变， 如分辨率为800x480的显示屏(800列，480行)，传输一帧的图像HSYNC的电平会跳变480次。

(4) 垂直同步信号VSYNC

> 垂直同步信号VSYNC(Vertical Sync)用于表示液晶屏一帧像素数据的传输结束，每传输完成一帧像素数据时，VSYNC会发生电平跳变。 其中“帧”是图像的单位，一幅图像称为一帧，在液晶屏中，一帧指一个完整屏液晶像素点。人们常常用“帧/秒”来表示液晶屏的刷新特性， 即液晶屏每秒可以显示多少帧图像，如液晶屏以60帧/秒的速率运行时，VSYNC每秒钟电平会跳变60次。

(5) 数据使能信号DE

> 数据使能信号DE(Data Enable)用于表示数据的有效性，当DE信号线为高电平时，RGB信号线表示的数据有效。

### 1.2 液晶数据传输时序

通过上述信号线向液晶屏传输像素数据时，各信号线的时序见图。图中表示的是向液晶屏传输一帧图像数据的时序，中间省略了多行及多个像素点。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD010.jpg)

液晶屏显示的图像可看作一个矩形，结合图来理解。液晶屏有一个显示指针，它指向将要显示的像素。 显示指针的扫描方向方向从左到右、从上到下，一个像素点一个像素点地描绘图形。这些像素点的数据通过RGB数据线传输至液晶屏， 它们在同步时钟CLK的驱动下一个一个地传输到液晶屏中，交给显示指针，传输完成一行时，水平同步信号HSYNC电平跳变一次，而传输完一帧时VSYNC电平跳变一次。

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD011.jpeg" title="" alt="" width="881">

但是，液晶显示指针在行与行之间，帧与帧之间切换时需要延时，而且HSYNC及VSYNC信号本身也有宽度， 这些时间参数说明见表

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD02.png)

在这些时间参数控制的区域，数据使能信号线“DE”都为低电平，RGB数据线的信号无效，当“DE”为高电平时，表示的数据有效，传输的数据会直接影响液晶屏的显示区域。

## 2. 使用STM32的FSMC模拟8080接口时序

ILI9341的8080通讯接口时序可以由STM32使用普通I/O接口进行模拟，但这样效率太低，STM32提供了一种特别的控制方法——使用FSMC接口实现8080时序。

### 2.1 FSMC简介

STM32F1系列芯片使用FSMC外设来管理扩展的存储器， FSMC是Flexible Static Memory Controller的缩写，译为灵活的静态存储控制器。 它可以用于驱动包括SRAM、NOR FLASH以及NAND FLSAH类型的存储器，不能驱动如SDRAM这种动态的存储器而在STM32F429系列的控制器中， 它具有FMC外设，支持控制SDRAM存储器。

由于FSMC外设可以用于控制扩展的外部存储器，而MCU对液晶屏的操作实际上就是把显示数据写入到显存中，与控制存储器非常类似， 且8080接口的通讯时序完全可以使用FSMC外设产生，因而非常适合使用FSMC控制液晶屏。

FSMC外设的结构见图：

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD016.jpeg" title="" alt="" width="1078">

#### 2.1.1 通讯引脚

在框图的右侧是FSMC外设相关的控制引脚，由于控制不同类型存储器的时候会有一些不同的引脚，看起来有非常多， 其中地址线FSMC_A和数据线FSMC_D是所有控制器都共用的。这些FSMC引脚具体对应的GPIO端口及引脚号可在《STM32F103规格书》中搜索查找到，不在此列出。

在本章示例中，控制LCD时，是使用FSMC的NORPSRAM模式的，而且控制LCD时使用的是NOR FLASH类型的模式B， 所以我们重点分析框图中NOR FLASH控制信号线部分，控制NOR FLASH主要使用到表中的信号线：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD04.png)

在控制LCD时，使用的是类似异步、地址与数据线独立的NOR FLASH控制方式，所以实际上CLK、NWAIT、NADV引脚并没有使用到。

其中比较特殊的FSMC_NE是用于控制存储器芯片的片选控制信号线，STM32具有FSMC_NE1/2/3/4号引脚，不同的引脚对应STM32内部不同的地址区域。 例如，当STM32访问0x68000000-0x6BFFFFFF地址空间时，FSMC_NE3引脚会自动设置为低电平，由于它一般连接到外部存储器的片选引脚且低电平有效， 所以外部存储器的片选被使能，而访问0x60000000-0x63FFFFFF地址时，FSMC_NE1会输出低电平。当使用不同的FSMC_NE引脚连接外部存储器时， STM32访问外部存储的地址不一样，从而达到控制多个外部存储器芯片的目的。

#### 2.1.2 存储器控制器

上面不同类型的引脚是连接到FSMC内部对应的存储控制器中的。NOR/PSRAM/SRAM设备使用相同的控制器，NAND/PC卡设备使用相同的控制器， 不同的控制器有专用的寄存器用于配置其工作模式。

控制NOR FLASH的有FSMC_BCR1/2/3/4控制寄存器、 FSMC_BTR1/2/3/4片选时序寄存器以及FSMC_BWTR1/2/3/4写时序寄存器。每种寄存器都有4个， 分别对应于4个不同的存储区域，各种寄存器介绍如下：

- FSMC_BCR控制寄存器可配置要控制的存储器类型、数据线宽度以及信号有效极性能参数。

- FMC_BTR时序寄存器用于配置SRAM访问时的各种时间延迟，如数据保持时间、地址保持时间等。

- FMC_BWTR写时序寄存器与FMC_BTR寄存器控制的参数类似，它专门用于控制写时序的时间参数。

#### 2.1.3 时钟控制逻辑

FSMC外设挂载在AHB总线上，时钟信号来自于HCLK(默认72MHz)，控制器的同步时钟输出就是由它分频得到。 例如，NOR控制器的FSMC_CLK引脚输出的时钟，它可用于与同步类型的NOR FLASH芯片进行同步通讯， 它的时钟频率可通过FSMC_BTR寄存器的CLKDIV位配置，可以配置为HCLK的1/2或1/3，也就是说，若它与同步类型的NOR FLASH通讯时， 同步时钟最高频率为36MHz。本示例中的NOR FLASH为异步类型的存储器，不使用同步时钟信号，所以时钟分频配置不起作用。

## 3. FSMC的地址映射

FSMC连接好外部的存储器并初始化后，就可以直接通过访问地址来读写数据，这种地址访问与I2C EEPROM、SPI FLASH的不一样， 后两种方式都需要控制I2C或SPI总线给存储器发送地址，然后获取数据；在程序里，这个地址和数据都需要分开使用不同的变量存储， 并且访问时还需要使用代码控制发送读写命令。而使用FSMC外接存储器时，其存储单元是映射到STM32的内部寻址空间的；在程序里， 定义一个指向这些地址的指针，然后就可以通过指针直接修改该存储单元的内容，FSMC外设会自动完成数据访问过程， 读写命令之类的操作不需要程序控制，访问示例代码：

```c
#define Bank1_SRAM3_ADDR    ((uint32_t)(0x68000000))
// 写/读 16位数据*/
*( uint16_t*) (Bank1_SRAM3_ADDR+10) = (uint16_t)0xBBBB;
printf("指针访问SRAM，写入数据0xBBBB \r\n");
temp =  *( uint16_t*) (Bank1_SRAM3_ADDR+10);
printf("读取数据：0x%X \r\n",temp);
```

以上代码实际上就是标准的C语言对特定地址的指针式访问，只是由于该地址被STM32映射到FSMC外设，所以访问这些地址时，FSMC会自动输出地址、数据等访问时序。

FSMC的地址映射见图。

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD017.jpeg" title="" alt="" width="844">

图中左侧的是Cortex-M3内核的存储空间分配，右侧是STM32 FSMC外设的地址映射。 可以看到FSMC的NOR/PSRAM/SRAM/NAND FLASH以及PC卡的地址都在External RAM地址空间内。正是因为存在这样的地址映射，使得访问FSMC控制的存储器时，就跟访问STM32的片上外设寄存器一样(片上外设的地址映射即图中左侧的“Peripheral”区域)。

FSMC把整个External RAM存储区域分成了4个Bank区域，并分配了地址范围及适用的存储器类型，如NOR及SRAM存储器只能使用Bank1的地址。 在每个Bank的内部又分成了4个小块，每个小块有相应的控制引脚用于连接片选信号，如FSMC_NE[4:1]信号线可用于选择BANK1内部的4小块地址区域， 见图。 当STM32访问0x6C000000-0x6FFFFFFF地址空间时，会访问到Bank1的第1小块区域，相应的FSMC_NE1信号线会输出控制信号。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD018.jpg)

## 4. FSMC控制异步NOR FLASH的时序

FSMC外设支持输出多种不同的时序以便于控制不同的存储器，它具有ABCD四种模式，下面我们仅针对控制异步NOR FLASH使用的模式B进行讲解， 见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD019.jpg)

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD020.jpg" title="" alt="" width="857">

当内核发出访问某个指向外部存储器地址时，FSMC外设会根据配置控制信号线产生时序访问存储器， 上图中的是访问外部异步NOR FLASH（模式B）时FSMC外设的读写时序。

以读时序为例，该图表示一个存储器操作周期由地址建立周期(ADDSET)、数据建立周期(DATAST)以及2个HCLK周期组成。在地址建立周期中， 地址线发出要访问的地址，数据掩码信号线指示出要读取地址的高、低字节部分，片选信号使能存储器芯片； 地址建立周期结束后读使能信号线发出读使能信号，接着存储器通过数据信号线把目标数据传输给FSMC，FSMC把它交给内核。

写时序类似，区别是它的一个存储器操作周期仅由地址建立周期(ADDSET)和数据建立周期(DATAST)组成，且在数据建立周期期间写使能信号线发出写信号， 接着FSMC把数据通过数据线传输到存储器中。

根据STM32对寻址空间的地址映射，地址0x6000 0000 ~0x9FFF FFFF是映射到外部存储器的， 而其中的0x6000 0000 ~0x6FFF FFFF则是分配给NOR FLASH、 PSRAM这类可直接寻址的器件。

当FSMC外设被配置成正常工作，并且外部接了NOR FLASH时， 若向0x60000000地址写入数据如0xABCD，FSMC会自动在各信号线上产生相应的电平信号， 写入数据。FSMC会控制片选信号NE1选择相应的NOR芯片，然后使用地址线A[25:0]输出0x60000000，在NWE写使能信号线上发出低电平的写使能信号， 而要写入的数据信号0xABCD则从数据线D[15:0]输出，然后数据就被保存到NOR FLASH中了。

## 5. 用FSMC模拟8080时序

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD021.jpeg)

见表对比FSMC NOR/PSRAM中的模式B时序与ILI9341液晶控制器芯片使用的8080时序可发现， 这两个时序是十分相似的(除了FSMC的地址线A和8080的D/CX线，可以说是完全一样)。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD05.png)

对于FSMC和8080接口，前四种信号线都是完全一样的，仅仅是FSMC的地址信号线A[25:0]与8080的数据/命令选择线D/CX有区别。而对于D/CX线， 它为高电平的时候表示数值，为低电平的时候表示命令，如果能使用FSMC的A地址线根据不同的情况产生对应的电平， 那么就完全可以使用FSMC来产生8080接口需要的时序了。

为了模拟出8080时序，我们可以把FSMC的A0地址线(也可以使用其它A1/A2等地址线)与ILI9341芯片8080接口的D/CX信号线连接， 那么当A0为高电平时(即D/CX为高电平)，数据线D[15:0]的信号会被ILI9341理解为数值，若A0为低电平时(即D/CX为低电平)， 传输的信号则会被理解为命令。

由于FSMC会自动产生地址信号，当使用FSMC向0x6xxx xxx1、0x6xxx xxx3、 0x6xxx xxx5…这些奇数地址写入数据时，地址最低位的值均为1， 所以它会控制地址线A0(D/CX)输出高电平，那么这时通过数据线传输的信号会被理解为数值； 若向0x6xxx xxx0 、0x6xxx xxx2、0x6xxx xxx4…这些偶数地址写入数据时， 地址最低位的值均为0，所以它会控制地址线A0(D/CX)输出低电平， 因此这时通过数据线传输的信号会被理解为命令，见表

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD06.png)

有了这个基础，只要配置好FSMC外设，然后在代码中利用指针变量，向不同的地址单元写入数据，就能够由FSMC模拟出的8080接口向ILI9341写入控制命令或GRAM的数据了。

注意：在实际控制时，以上地址计算方式还不完整，还需要注意HADDR内部地址与FSMC地址信号线的转换，关于这部分内容在代码讲解时再详细举例说明。

## 6. NOR FLASH时序结构体

在讲解程序前，再来了解一下与FSMC NOR FLASH控制相关的结构体。

控制FSMC使用NOR FLASH存储器时主要是配置时序寄存器以及控制寄存器，利用ST标准库的时序结构体以及初始化结构体可以很方便地写入参数。

NOR FLASH时序结构体的成员见下：

```c
typedef struct
{
uint32_t FSMC_AddressSetupTime;     // 地址建立时间，0-0xF个HCLK周期
uint32_t FSMC_AddressHoldTime;      // 地址保持时间，0-0xF个HCLK周期
uint32_t FSMC_DataSetupTime;        // 地址建立时间，0-0xF个HCLK周期
uint32_t FSMC_BusTurnAroundDuration;// 总线转换周期,0-0xF个HCLK周期，在NOR FLASH 
uint32_t FSMC_CLKDivision;          // 时钟分频因子,1-0xF，若控制异步存储器，本参数无效 
uint32_t FSMC_DataLatency;          // 数据延迟时间，若控制异步存储器，本参数无效 
uint32_t FSMC_AccessMode;           // 设置访问模式
}FSMC_NORSRAMTimingInitTypeDef;
```

下面我们详细介绍一下这个结构体：

**FSMC_BusWidth**

- **类型**: `uint32_t`
- **描述**: 这用于设置总线宽度，通常有 8 位、16 位和 32 位可选。选择合适的总线宽度可以提高数据传输速度。
- **示例**:

```c
FSMC_BusWidth = FSMC_DataWidth_16b; // 设置数据总线宽度为16位
```

**FSMC_WaitSignalPolarity**

- **类型**: `uint32_t`
- **描述**: 设置等待信号的极性，通常有高电平和低电平两种选择。这个参数影响外部存储器在等待信号时的行为。
- **示例**:

```c
FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_High; // 设置等待信号为高电平
```

**FSMC_WaitSignalActive**

- **类型**: `uint32_t`
- **描述**: 表示在访问期间，等待信号的活动状态（在读或写操作时）。
- **示例**:

```c
FSMC_WaitSignalActive = FSMC_WaitSignalActive_LastCycle; // 等待信号在最后一个周期有效
```

**FSMC_WriteOperation**

- **类型**: `uint32_t`
- **描述**: 设置写操作的配置，指明是否启用写操作。
- **示例**:

```c
FSMC_WriteOperation = FSMC_WriteOperation_Enable; // 启用写操作
```

**FSMC_ReadWriteTiming**

- **类型**: `FSMC_NORSRAMTimingInitTypeDef`
- **描述**: 用于分别配置读和写的时序参数。允许对读写操作进行独立配置，以优化性能。
- **示例**:

```c
FSMC_ReadWriteTiming.FSMC_AddressSetupTime = 2; // 读操作地址建立时间
FSMC_ReadWriteTiming.FSMC_DataSetupTime = 3; // 读操作数据建立时间
```

完整配置示例代码：

```c
// 定义 FSMC 时序结构体
FSMC_NORSRAMTimingInitTypeDef FSMC_Timing;
// 配置地址建立时间
FSMC_Timing.FSMC_AddressSetupTime = 2; // 地址建立时间设置为2个时钟周期
// 配置地址保持时间
FSMC_Timing.FSMC_AddressHoldTime = 1; // 地址保持时间设置为1个时钟周期
// 配置数据建立时间
FSMC_Timing.FSMC_DataSetupTime = 3; // 数据建立时间设置为3个时钟周期
// 配置总线转移持续时间
FSMC_Timing.FSMC_BusTurnAroundDuration = 1; // 总线转移时间设置为1个时钟周期
// 配置时钟分频
FSMC_Timing.FSMC_CLKDivision = 1; // 时钟分频设置为1
// 配置数据延迟
FSMC_Timing.FSMC_DataLatency = 0; // 数据延迟设置为0
// 配置访问模式
FSMC_Timing.FSMC_AccessMode = FSMC_AccessMode_A; // 设置访问模式为A
// 定义 FSMC 初始化结构体
FSMC_NORSRAMInitTypeDef FSMC_Init;
// 选择bank
FSMC_Init.FSMC_Bank = FSMC_Bank1_NORSRAM1; // 选择第1个NORSRAM
// 启用地址数据复用
FSMC_Init.FSMC_DataAddressMux = FSMC_DataAddressMux_Enable; // 启用地址和数据复用
// 配置存储器类型为NOR
FSMC_Init.FSMC_MemoryType = FSMC_MemoryType_NOR; // 设置存储器类型为NOR
// 配置数据宽度为16位
FSMC_Init.FSMC_MemoryDataWidth = FSMC_DataWidth_16b; // 设置数据总线宽度为16位
// 启用写操作
FSMC_Init.FSMC_WriteOperation = FSMC_WriteOperation_Enable; // 启用写操作
// 配置等待信号极性
FSMC_Init.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_High; // 设置等待信号为高电平
// 配置等待信号活动状态
FSMC_Init.FSMC_WaitSignalActive = FSMC_WaitSignalActive_LastCycle; // 等待信号在最后一个周期有效
// 配置读写时序
FSMC_Init.FSMC_ReadWriteTiming = &FSMC_Timing; // 设置读写时序为上面配置的时序
FSMC_Init.FSMC_WriteTiming = &FSMC_Timing; // 设置写时序为上面配置的时序

// 调用初始化函数以完成配置
FSMC_NORSRAMInit(&FSMC_Init);
```

## 7. FSMC初始化结构体

```c
typedef struct
{
uint32_t FSMC_Bank;              // 设置要控制的Bank区域 
uint32_t FSMC_DataAddressMux;    // 设置地址总线与数据总线是否复用 
uint32_t FSMC_MemoryType;        // 设置存储器的类型 
uint32_t FSMC_MemoryDataWidth;   // 设置存储器的数据宽度
uint32_t FSMC_BurstAccessMode;   // 设置是否支持突发访问模式，只支持同步类型的存储器 
uint32_t FSMC_AsynchronousWait;  //设置是否使能在同步传输时的等待信号，
uint32_t FSMC_WaitSignalPolarity;// 设置等待信号的极性
uint32_t FSMC_WrapMode;          // 设置是否支持对齐的突发模式 
uint32_t FSMC_WaitSignalActive;  // 配置等待信号在等待前有效还是等待期间有效 
uint32_t FSMC_WriteOperation;    // 设置是否写使能
uint32_t FSMC_WaitSignal;        // 设置是否使能等待状态插入 
uint32_t FSMC_ExtendedMode;      // 设置是否使能扩展模式 
uint32_t FSMC_WriteBurst;        // 设置是否使能写突发操作
// 当不使用扩展模式时，本参数用于配置读写时序，否则用于配置读时序
FSMC_NORSRAMTimingInitTypeDef* FSMC_ReadWriteTimingStruct;
// 当使用扩展模式时，本参数用于配置写时序
FSMC_NORSRAMTimingInitTypeDef* FSMC_WriteTimingStruct;
}FSMC_NORSRAMInitTypeDef;
```

这个结构体，除最后两个成员是上一小节讲解的时序配置外，其它结构体成员的配置都对应到FSMC_BCR中的寄存器位。各个成员意义介绍如下：

**FSMC_Bank**

- **意义**: 选择要使用的 FSMC 区域块。
- **示例**：

```c
FSMC_Bank1;  // 选择 FSMC 
```

**FSMC_DataAddressMux**

- **意义**: 确定地址总线和数据总线是否复用。
- **示例**:

```c
FSMC_DataAddressMux_Enable;  // 启用地址和数据复用
FSMC_DataAddressMux_Disable; // 禁用地址和数据复用
```

**FSMC_MemoryType**

- **意义**: 指定连接的存储器类型（如 SRAM、NOR Flash）。
- **示例**:

```c
FSMC_MemoryType_SRAM; // 指定为 SRAM
FSMC_MemoryType_NOR;  // 指定为 NOR Flash
```

**FSMC_MemoryDataWidth**

- **意义**: 设置存储器的数据宽度（8 位或 16 位）。
- **示例**：

```c
FSMC_MemoryDataWidth_8b;    // 8 位数据宽度
FSMC_MemoryDataWidth_16b;   // 16 位数据宽度
```

**FSMC_BurstAccessMode**

- **意义**: 启用或禁用突发访问模式。
- **示例**:

```c
FSMC_BurstAccessMode_Enable; // 启用突发访问模式
FSMC_BurstAccessMode_Disable;// 禁用突发访问模式
```

**FSMC_AsynchronousWait**

- **意义**: 配置是否在同步传输中使能等待信号。

- **示例**：

```c
FSMC_AsynchronousWait_Enable; // 启用异步等待
FSMC_AsynchronousWait_Disable;// 禁用异步等待
```

**FSMC_WaitSignalPolarity**

- **意义**: 设置等待信号的极性（高电平或低电平有效）。

- **示例**:

```c
FSMC_WaitSignalPolarity_High; // 等待信号高有效
FSMC_WaitSignalPolarity_Low;  // 等待信号低有效
```

**FSMC_WrapMode**

- **意义**: 设置是否支持突发模式下的循环操作。
- **示例**:

```c
FSMC_WrapMode_Enable; // 启用循环模式
FSMC_WrapMode_Disable;// 禁用循环模式
```

**FSMC_WaitSignalActive**

- **意义**: 决定等待信号的有效时机。
- **示例**:

```c
FSMC_WaitSignalActive_BeforeWaitState; // 在等待状态之前有效
FSMC_WaitSignalActive_DuringWaitState; // 在等待状态期间有效
```

**FSMC_WriteOperation**

- **意义**: 设置是否允许写操作。
- **示例**:

```c
FSMC_WriteOperation_Enable; // 启用写操作
FSMC_WriteOperation_Disable;// 禁用写操作
```

**FSMC_WaitSignal**

- **意义**: 配置是否在读写过程中插入等待状态。
- **示例**:

```c
FSMC_WaitSignal_Enable; // 启用等待信号
FSMC_WaitSignal_Disable;// 禁用等待信号
```

**FSMC_ExtendedMode**

- **意义**: 启用额外的扩展配置选项。
- **示例**:

```c
FSMC_ExtendedMode_Enable; // 启用扩展模式
FSMC_ExtendedMode_Disable;// 禁用扩展模式
```

**FSMC_WriteBurst**

- **意义**: 启用写入突发操作。
- **示例**:

```c
FSMC_WriteBurst_Enable; // 启用写入突发
FSMC_WriteBurst_Disable;// 禁用写入突发
```

以下是一个完整的示例，展示如何配置 `FSMC_NORSRAMInitTypeDef` 结构体：

```c
FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStruct;

// 配置 FSMC 参数
FSMC_NORSRAMInitStruct.FSMC_Bank = FSMC_Bank1;
FSMC_NORSRAMInitStruct.FSMC_DataAddressMux = FSMC_DataAddressMux_Enable;
FSMC_NORSRAMInitStruct.FSMC_MemoryType = FSMC_MemoryType_NOR;
FSMC_NORSRAMInitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
FSMC_NORSRAMInitStruct.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Enable;
FSMC_NORSRAMInitStruct.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
FSMC_NORSRAMInitStruct.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_High;
FSMC_NORSRAMInitStruct.FSMC_WrapMode = FSMC_WrapMode_Disable;
FSMC_NORSRAMInitStruct.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
FSMC_NORSRAMInitStruct.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
FSMC_NORSRAMInitStruct.FSMC_WaitSignal = FSMC_WaitSignal_Enable;
FSMC_NORSRAMInitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
FSMC_NORSRAMInitStruct.FSMC_WriteBurst = FSMC_WriteBurst_Enable;
// 初始化 FSMC
FSMC_NORSRAMInit(&FSMC_NORSRAMInitStruct); 
// 这个示例展示了如何配置 FSMC 控制器以连接到一个 16 位宽的 NOR Flash 存储器，
// 启用突发访问和写操作。
```

---

2024.9.26 第一次修订，后期不再维护

2025.2.1 简化内容
