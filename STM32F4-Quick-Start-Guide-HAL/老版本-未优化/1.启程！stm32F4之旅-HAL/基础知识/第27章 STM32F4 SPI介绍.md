# 第二十七章 STM32F4 SPI介绍

## 1. SPI简介

SPI 是英语 Serial Peripheral interface 的缩写，顾名思义就是串行外围设备接口。是 Motorola首先在其 MC68HCXX 系列处理器上定义的。 SPI 接口主要应用在 EEPROM， FLASH，实时时钟， AD 转换器，还有数字信号处理器和数字信号解码器之间。 SPI，是一种高速的，全双工，同步的通信总线，并且在芯片的管脚上只占用四根线，节约了芯片的管脚，同时为 PCB 的布局上节省空间，提供方便，正是出于这种简单易用的特性，现在越来越多的芯片集成了这种通信协议， STM32F4 也有 SPI 接口。

![屏幕截图 2024-10-20 142207.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/20-14-22-24-屏幕截图%202024-10-20%20142207.png)

SPI 接口一般使用 4 条线通信：

- MISO 主设备数据输入，从设备数据输出。

- MOSI 主设备数据输出，从设备数据输入。

- SCLK 时钟信号，由主设备产生。

- CS 从设备片选信号，由主设备控制。

从图中可以看出， 主机和从机都有一个串行移位寄存器，主机通过向它的 SPI 串行寄存器写入一个字节来发起一次传输。寄存器通过 MOSI 信号线将字节传送给从机，从机也将自己的移位寄存器中的内容通过 MISO 信号线返回给主机。这样，两个移位寄存器中的内容就被交换。外设的写操作和读操作是同步完成的。如果只进行写操作，主机只需忽略接收到的字节；反之，若主机要读取从机的一个字节，就必须发送一个空字节来引发从机的传输。

SPI 主要特点有： 可以同时发出和接收串行数据； 可以当作主机或从机工作； 提供频率可编程时钟； 发送结束中断标志； 写冲突保护； 总线竞争保护等。

STM32F4 的 SPI 功能很强大， SPI 时钟最高可以到 37.5Mhz，支持 DMA，可以配置为 SPI协议或者 I2S 协议（支持全双工 I2S）。

这节，我们使用 STM32F4 的 SPI1 的主模式，下面就来看看 SPI1 部分的设置步骤吧。 SPI相关的库函数和定义分布在文件 stm32f4xx_spi.c 以及头文件 stm32f4xx_spi.h 中。 STM32 的主模式配置步骤如下：

## 2. SPI配置基本步骤

### 2.1 配置相关引脚的复用功能，使能SPI1时钟

我们要用 SPI1，第一步就要使能 SPI1 的时钟， SPI1 的时钟通过 APB2ENR 的第 12 位来设置。其次要设置 SPI1 的相关引脚为复用(AF5)输出，这样才会连接到 SPI1 上。这里我们使用的是 PB3、 4、 5 这 3 个（SCK.、 MISO、 MOSI， CS 使用软件管理方式），所以设置这三个为复用IO，复用功能为 AF5。

使能SPI1时钟的方法为：

```c
__HAL_RCC_SPI1_CLK_ENABLE(); // 使能 SPI1 时钟
```

复用 PB3,PB4,PB5 为 SPI1 引脚通过 HAL_GPIO_Init 函数实现，代码如下：

```c
GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
GPIO_Initure.Mode=GPIO_MODE_AF_PP; // 复用推挽输出
GPIO_Initure.Pull=GPIO_PULLUP;     // 上拉
GPIO_Initure.Speed=GPIO_SPEED_FAST;// 快速
GPIO_Initure.Alternate=GPIO_AF5_SPI1;// 复用为 SPI1
HAL_GPIO_Init(GPIOB,&GPIO_Initure);
```

### 2.2 初始化SPI1，设置SPI1工作模式等

这一步全部是通过 SPI1_CR1 来设置，我们设置 SPI1 为主机模式，设置数据格式为 8 位，然后通过 CPOL 和 CPHA 位来设置 SCK 时钟极性及采样方式。并设置 SPI1 的时钟频率（最大37.5Mhz），以及数据的格式（MSB 在前还是 LSB 在前）。 在 HAL 库中初始化 SPI 的函数为：

```c
HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi);
```

下面我们来看看 SPI_HandleTypeDef 定义：

```c
typedef struct __SPI_HandleTypeDef
{
    SPI_TypeDef *Instance;// 基地址
    SPI_InitTypeDef Init; // 初始化接哦固体
    uint8_t *pTxBuffPtr;  // 发送缓存
    uint16_t TxXferSize;  // 发送数据大小
    uint16_t TxXferCount; // 还剩余多少个数据要发送
    uint8_t *pRxBuffPtr;  // 接收缓存
    uint16_t RxXferSize;  // 接收数据大小
    uint16_t RxXferCount; // 还剩余多少个数据要接收
    DMA_HandleTypeDef *hdmatx; // DMA 发送句柄
    DMA_HandleTypeDef *hdmarx; // DMA 接收句柄
    void (*RxISR)(struct __SPI_HandleTypeDef * hspi);
    void (*TxISR)(struct __SPI_HandleTypeDef * hspi);
    HAL_LockTypeDef Lock;
    __IO HAL_SPI_StateTypeDef State;
    __IO uint32_t ErrorCode;
}SPI_HandleTypeDef;
```

该结构体和串口句柄结构体类似，同样有 6 个成员变量和 2 个 DMA_HandleTypeDef 指针类型变量。这几个参数的作用这里我们就不做过多讲解，大家如果对 HAL 库串口通信理解了，那么这些就很好理解。这里我们主要讲解第二个成员变量 Init，它是 SPI_InitTypeDef 结构体类型，该结构体定义如下：

```c
typedef struct
{
    uint32_t Mode;     // 模式：主（SPI_MODE_MASTER），从（SPI_MODE_SLAVE）
    uint32_t Direction;// 方式： 只接受模式， 单线双向通信数据模式，全双工
    uint32_t DataSize; // 8 位还是 16 位帧格式选择项
    uint32_t CLKPolarity;// 时钟极性
    uint32_t CLKPhase;   // 时钟相位
    uint32_t NSS;        // SS 信号由硬件（NSS 管脚）还是软件控制
    uint32_t BaudRatePrescaler; // 设置 SPI 波特率预分频值
    uint32_t FirstBit; // 起始位是 MSB 还是 LSB
    uint32_t TIMode;   // 帧格式 SPI motorola 模式还是 TI 模式
    uint32_t CRCCalculation;// 硬件 CRC 是否使能
    uint32_t CRCPolynomial; // CRC 多项式
}SPI_InitTypeDef;
```

该结构体个个成员变量的含义我们已经在成员变量后面注释了，请大家参考学习。 SPI 初始化实例代码如下：

```c
SPI1_Handler.Instance=SPI1; //SPI1
SPI1_Handler.Init.Mode=SPI_MODE_MASTER; //设置 SPI 工作模式，设置为主模式
SPI1_Handler.Init.Direction=SPI_DIRECTION_2LINES;
//设置 SPI 单向或者双向的数据模式:SPI 设置为双线模式
SPI1_Handler.Init.DataSize=SPI_DATASIZE_8BIT;
//设置 SPI 的数据大小:SPI 发送接收 8 位帧结构
SPI1_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;
//串行同步时钟的空闲状态为高电平
SPI1_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;
//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
SPI1_Handler.Init.NSS=SPI_NSS_SOFT; //NSS 信号由硬件（NSS 管脚）还是软件
//（使用 SSI 位）管理:内部 NSS 信号有 SSI 位控制
SPI1_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;
//定义波特率预分频的值:波特率预分频值为 256
SPI1_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;
//指定数据传输从 MSB 位还是 LSB 位开始:数据传输从 MSB 位开始
SPI1_Handler.Init.TIMode=SPI_TIMODE_DISABLE; //关闭 TI 模式
SPI1_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;
//关闭硬件 CRC 校验
SPI1_Handler.Init.CRCPolynomial=7; //CRC 值计算的多项式
HAL_SPI_Init(&SPI1_Handler);//初始化
```

同样， HAL 库也提供了 SPI 初始化 MSP 回调函数 HAL_SPI_MspInit，定义如下：

```c
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi);
```

### 2.3 使能SPI1

这一步通过 SPI1_CR1 的 bit6 来设置，以启动 SPI1，在启动之后，我们就可以开始 SPI 通讯了。 库函数使能 SPI1 的方法为：

```c
__HAL_SPI_ENABLE(&SPI1_Handler); // 使能 SPI1
```

### 2.4 SPI传输数据

通信接口当然需要有发送数据和接受数据的函数， HAL 库提供的发送数据函数原型为：

```c
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                   uint16_t Size, uint32_t Timeout);
```

这个函数很好理解，往 SPIx 数据寄存器写入数据 Data，从而实现发送。

HAL 库提供的接受数据函数原型为：

```c
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData,
                                  uint16_t Size, uint32_t Timeout);
```

这个函数也不难理解，从 SPIx 数据寄存器读出接受到的数据。

前面我们讲解了 SPI 通信的原理，因为 SPI 是全双工，发送一个字节的同时接受一个字节，发送和接收同时完成，所以 HAL 也提供了一个发送接收统一函数：

```c
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData,
                             uint8_t *pRxData, uint16_t Size, uint32_t Timeout);
```

### 2.5 设置SPI传输速度

SPI 初始化结构体 SPI_InitTypeDef 有一个成员变量是 BaudRatePrescaler，该成员变量用来设置 SPI 的预分频系数，从而决定了 SPI 的传输速度。但是 HAL 库并没有提供单独的 SPI 分频系数修改函数，如果我们需要在程序中不时的修改速度，那么我们就要通过设置 SPI 的 CR1 寄存器来修改，具体实现方法请参考后面软件设计小节相关函数。

## 3. W25Q128介绍

W25Q128 是华邦公司推出的大容量 SPI FLASH 产品， W25Q128 的容量为 128Mb， 该系列还有 W25Q80/16/32/64 等。 ALIENTEK所选择的 W25Q128 容量为 128Mb，也就是 16M 字节。

W25Q128 将 16M 的容量分为 256 个块（Block），每个块大小为 64K 字节，每个块又分为16 个扇区（Sector），每个扇区 4K 个字节。 W25Q128 的最小擦除单位为一个扇区，也就是每次必须擦除 4K 个字节。这样我们需要给 W25Q128 开辟一个至少 4K 的缓存区，这样对 SRAM 要求比较高，要求芯片必须有 4K 以上 SRAM 才能很好的操作。

W25Q128 的擦写周期多达 10W 次，具有 20 年的数据保存期限，支持电压为 2.7~3.6V， W25Q128 支持标准的 SPI，还支持双输出/四输出的 SPI，最大 SPI 时钟可以到 80Mhz（双输出时相当于 160Mhz，四输出时相当于 320M）


