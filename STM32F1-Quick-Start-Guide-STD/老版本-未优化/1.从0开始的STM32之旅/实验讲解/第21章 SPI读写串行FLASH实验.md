# 第二十一章 SPI读写串行FLASH实验

## 1. 硬件设计

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI008.png" title="" alt="" width="603">

实验板中的FLASH芯片(型号：W25Q64)是一种使用SPI通讯协议的NOR FLASH存储器， 它的CS/CLK/DIO/DO引脚分别连接到了STM32对应的SPI引脚NSS/SCK/MOSI/MISO上，其中STM32的NSS引脚是一个普通的GPIO， 不是SPI的专用NSS引脚，所以程序中我们要使用软件控制的方式。

FLASH芯片中还有WP和HOLD引脚。WP引脚可控制写保护功能，当该引脚为低电平时，禁止写入数据。 我们直接接电源，不使用写保护功能。HOLD引脚可用于暂停通讯，该引脚为低电平时，通讯暂停， 数据输出引脚输出高阻抗状态，时钟和数据输入引脚无效。我们直接接电源，不使用通讯暂停功能。

## 2. 软件设计

### 2.1 编程大纲

1. W25Q64相关参数宏定义

2. SPI接口相关参数宏定义

3. 初始化SPI，配置SPI工作模式

4. 编写SPI基本操作函数

5. 编写FLASH基本操作函数

6. 编写测试程序，对读写数据进行校验。

### 2.2 代码分析

#### 2.2.1 W25Q64相关参数宏定义

```c
/* 超时时间 */
#define SPIT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT ((uint32_t)(10*SPIT_FLAG_TIMEOUT))

/* W25QXX ID */
#define sFLASH_ID 0xEF4017 // W25Q64
// #define sFLASH_ID 0xEF4018 // W25Q128
// #define sFLASH_ID 0xEF4015 // W25Q16
// #define sFALSH_ID 0xEF3015 // W25X16

#define SPI_FLASH_PageSize 256 // W25Q64页大小为256字节
#define SPI_FLASH_PerWritePageSize 256 // W25Q64一次写操作最大为256字节

/* W25QXX命令 */
#define W25X_WriteEnable            0x06 // 写使能 
#define W25X_WriteDisable            0x04 // 写禁止 
#define W25X_ReadStatusReg            0x05 // 读取状态寄存器 
#define W25X_WriteStatusReg            0x01 // 写入状态寄存器
#define W25X_ReadData                0x03 // 读取数据 
#define W25X_FastReadData            0x0B // 快速读取数据 
#define W25X_FastReadDual            0x3B // 快速读取双线 
#define W25X_PageProgram            0x02 // 页编程 
#define W25X_BlockErase                0xD8 // 块擦除 
#define W25X_SectorErase            0x20 // 扇区擦除 
#define W25X_ChipErase                0xC7 // 全片擦除 
#define W25X_PowerDown                0xB9 // 掉电 
#define W25X_ReleasePowerDown        0xAB // 复位掉电 
#define W25X_DeviceID                0xAB // 读取设备ID 
#define W25X_ManufactDeviceID       0x90 // 读取厂商ID 
#define W25X_JedecDeviceID            0x9F // 读取Jedec ID 
/* WIP(BUSY)标志，表示FALSH内部在写入数据 */
#define WIP_Flag              0x01
#define Dummy_Byte            0xFF

/* 信息输出 */
#define FLASH_DEBUG_ON         1
#define FLASH_INFO(fmt,arg...)           printf("<<-FLASH-INFO->> "fmt"\n",##arg)
#define FLASH_ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define FLASH_DEBUG(fmt,arg...)          do{\
                                          if(FLASH_DEBUG_ON)\
                                          printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)
```

#### 2.2.2 SPI接口相关参数宏定义

```c
/* SPI接口配置宏定义 */
// 1.SPI基础配置
#define FLASH_SPIx SPI1
#define FLASH_SPI_APBxClock_FUN RCC_APB2PeriphClockCmd
#define FLASH_SPI_CLK RCC_APB2Periph_SPI1
// 2.CS引脚配置(PC0)
#define FLASH_SPI_CS_APBxClock_FUN RCC_APB2PeriphClockCmd
#define FLASH_SPI_CS_CLK RCC_APB2Periph_GPIOC
#define FLASH_SPI_CS_PORT GPIOC
#define FLASH_SPI_CS_PIN GPIO_Pin_0
// 3.SCK引脚配置(PA5)
#define FLASH_SPI_SCK_APBxClock_FUN RCC_APB2PeriphClockCmd
#define FLASH_SPI_SCK_CLK RCC_APB2Periph_GPIOA
#define FLASH_SPI_SCK_PORT GPIOA
#define FLASH_SPI_SCK_PIN GPIO_Pin_5
// 4.MISO引脚配置(PA6)
#define FLASH_SPI_MISO_APBxClock_FUN RCC_APB2PeriphClockCmd
#define FLASH_SPI_MISO_CLK RCC_APB2Periph_GPIOA
#define FLASH_SPI_MISO_PORT GPIOA
#define FLASH_SPI_MISO_PIN GPIO_Pin_6
// 5.MOSI引脚配置(PA7)
#define FLASH_SPI_MOSI_APBxClock_FUN RCC_APB2PeriphClockCmd
#define FLASH_SPI_MOSI_CLK RCC_APB2Periph_GPIOA
#define FLASH_SPI_MOSI_PORT GPIOA
#define FLASH_SPI_MOSI_PIN GPIO_Pin_7
// 6.CS高低电平执行函数
#define SPI_FLASH_CS_LOW() GPIO_ResetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)
#define SPI_FLASH_CS_HIGH() GPIO_SetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)
```

#### 2.2.3 利用上面的宏，编写SPI的初始化函数

```c
// SPI_FLASH接口初始化
void SPI_FLASH_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  // 使能时钟
  FLASH_SPI_APBxClock_FUN(FLASH_SPI_CLK, ENABLE); // 使能SPI时钟
  FLASH_SPI_CS_APBxClock_FUN(FLASH_SPI_CS_CLK|FLASH_SPI_SCK_CLK|FLASH_SPI_MOSI_CLK|FLASH_SPI_MISO_CLK, ENABLE); // 使能SPI的接口引脚时钟
  // 配置GPIO
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStructure);  // 初始化CS引脚
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStructure); // 初始化SCK引脚
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MOSI_PIN;
  GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStructure);// 初始化MOSI引脚
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN;
  GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStructure);// 初始化MISO引脚
  SPI_FLASH_CS_HIGH(); // CS引脚置高，FLASH停止信号
  // SPI工作模式配置
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 全双工模式
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; // 主机模式
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // 8位数据模式
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; // 时钟极性高
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; // 第二个时钟沿采样
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; // 软件NSS管理
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // 时钟预分频器
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // MSB位先行
  SPI_InitStructure.SPI_CRCPolynomial = 7; // CRC校验位
  SPI_Init(FLASH_SPIx, &SPI_InitStructure); // 初始化SPI
  SPI_Cmd(FLASH_SPIx, ENABLE); // 使能SPI
}
```

与所有使用到GPIO的外设一样，都要先把使用到的GPIO引脚模式初始化，配置好复用功能。GPIO初始化流程如下：

1. 使用GPIO_InitTypeDef定义GPIO初始化结构体变量， 以便下面用于存储GPIO配置；

2. 调用库函数RCC_APB2PeriphClockCmd 来使能SPI引脚使用的GPIO端口时钟。

3. 向GPIO初始化结构体赋值，把SCK/MOSI/MISO引脚初始化成复用推挽模式。 而CS(NSS)引脚由于使用软件控制，我们把它配置为普通的推挽输出模式。

4. 使用以上初始化结构体的配置，调用GPIO_Init函数向寄存器写入参数， 完成GPIO的初始化。

以上只是配置了SPI使用的引脚，对SPI外设模式的配置。在配置STM32的SPI模式前，我们要先了解从机端的SPI模式。

根据FLASH芯片的说明，它支持SPI模式0及模式3，支持双线全双工， 使用MSB先行模式，支持最高通讯时钟为104MHz，数据帧长度为8位。我们要把STM32的SPI外设中的这些参数配置一致。

这段代码中，把STM32的SPI外设配置为主机端，双线全双工模式，数据帧长度为8位，使用SPI模式3(CPOL=1，CPHA=1)， NSS引脚由软件控制以及MSB先行模式。代码中把SPI的时钟频率配置成了4分频，实际上可以配置成2分频以提高通讯速率， 读者可亲自尝试一下。最后一个成员为CRC计算式，由于我们与FLASH芯片通讯不需要CRC校验，并没有使能SPI的CRC功能， 这时CRC计算式的成员值是无效的。

赋值结束后调用库函数SPI_Init把这些配置写入寄存器，并调用SPI_Cmd函数使能外设。

如果之前对结构体各参数不理解，这里我们还可以再讲诉一遍：

`SPI_InitTypeDef`结构体的参数和意义

1. **`SPI_Direction`**:
   
   `SPI_Direction = SPI_Direction_2Lines_FullDuplex;`
   
   - **SPI_Direction_2Lines_FullDuplex**: SPI工作在全双工模式下，数据可以在两个方向上同时传输。
   - 其他可选项：`SPI_Direction_1Line_Rx`（接收单线模式），`SPI_Direction_1Line_Tx`（发送单线模式）。

2. **`SPI_Mode`**:
   
   `SPI_Mode = SPI_Mode_Master;`
   
   - **SPI_Mode_Master**: SPI工作在主模式。主设备生成时钟信号并控制通信。
   - 其他可选项：`SPI_Mode_Slave`（从模式）。在从模式下，SPI设备不会生成时钟信号。

3. **`SPI_DataSize`**:
   
   `SPI_DataSize = SPI_DataSize_8b;`
   
   - **SPI_DataSize_8b**: 数据传输的位宽为8位。
   - 其他可选项：`SPI_DataSize_16b`（16位数据宽度）。

4. **`SPI_CPOL`**:
   
   `SPI_CPOL = SPI_CPOL_High;`
   
   - **SPI_CPOL_High**: SPI时钟的空闲状态为高电平。
   - 其他可选项：`SPI_CPOL_Low`（时钟空闲状态为低电平）。

5. **`SPI_CPHA`**:
   
   `SPI_CPHA = SPI_CPHA_2Edge;`
   
   - **SPI_CPHA_2Edge**: 数据在时钟的第二个边沿（下降沿）采样。
   - 其他可选项：`SPI_CPHA_1Edge`（数据在时钟的第一个边沿（上升沿）采样）。

6. **`SPI_NSS`**:
   
   `SPI_NSS = SPI_NSS_Soft;`
   
   - **SPI_NSS_Soft**: 使用软件管理NSS（片选）信号。
   - 其他可选项：`SPI_NSS_Hard`（硬件管理NSS信号）。

7. **`SPI_BaudRatePrescaler`**:
   
   `SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;`
   
   - **SPI_BaudRatePrescaler_4**: 设置SPI时钟的预分频因子为4。时钟频率是系统时钟的四分之一。
   - 其他可选项：`SPI_BaudRatePrescaler_2`, `SPI_BaudRatePrescaler_8`, `SPI_BaudRatePrescaler_16`, `SPI_BaudRatePrescaler_32`, `SPI_BaudRatePrescaler_64`, `SPI_BaudRatePrescaler_128`, `SPI_BaudRatePrescaler_256`（不同的预分频因子，影响SPI的实际时钟频率）。

8. **`SPI_FirstBit`**:
   
   `SPI_FirstBit = SPI_FirstBit_MSB;`
   
   - **SPI_FirstBit_MSB**: 数据传输时高位在前。
   - 其他可选项：`SPI_FirstBit_LSB`（数据传输时低位在前）。

9. **`SPI_CRCPolynomial`**:
   
   `SPI_CRCPolynomial = 7;`
   
   - **SPI_CRCPolynomial**: CRC的多项式值，用于数据的完整性校验。

#### 2.2.4 SPI基本操作函数

初始化好SPI外设后，就可以使用SPI通讯了，复杂的数据通讯都是由单个字节数据收发组成的， 我们看看它的代码实现

```c
// 使用SPI发送一个字节
uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
  SPITimeout = SPIT_FLAG_TIMEOUT; // 超时时间初始化
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE) == RESET) // 等待发送缓冲区空(TXE事件)
  {
    if((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(0);
  }
  SPI_I2S_SendData(FLASH_SPIx, byte); // 写入数据寄存器
  SPITimeout = SPIT_FLAG_TIMEOUT;
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE) == RESET) // 等待接收缓冲区非空(RXNE事件)
  {
    if((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(1);
  }
  return SPI_I2S_ReceiveData(FLASH_SPIx); // 读取数据寄存器
}

// 使用SPI发送两个字节
uint16_t SPI_FLASH_SendHalfWord(uint16_t halfword)
{
  SPITimeout = SPIT_FLAG_TIMEOUT; // 超时时间初始化
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE) == RESET) // 等待发送缓冲区空(TXE事件)
  {
    if((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(2);
  }
  SPI_I2S_SendData(FLASH_SPIx, halfword); // 写入数据寄存器
  SPITimeout = SPIT_FLAG_TIMEOUT;
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE) == RESET) // 等待接收缓冲区非空(RXNE事件)
  {
    if((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(3);
  }
  return SPI_I2S_ReceiveData(FLASH_SPIx); // 读取数据寄存器
}

// 使用SPI读取一个字节
uint8_t SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}
```

SPI_FLASH_SendByte发送单字节函数中包含了等待事件的超时处理，这部分原理跟I2C中的一样，在此不再赘述。

SPI_FLASH_SendByte函数实现了前面讲解的“SPI通讯过程”：

1. 本函数中不包含SPI起始和停止信号，只是收发的主要过程， 所以在调用本函数前后要做好起始和停止信号的操作；

2. 对SPITimeout变量赋值为宏SPIT_FLAG_TIMEOUT。这个SPITimeout变量在下面的while循环中每次循环减1， 该循环通过调用库函数SPI_I2S_GetFlagStatus检测事件，若检测到事件，则进入通讯的下一阶段， 若未检测到事件则停留在此处一直检测，当检测SPIT_FLAG_TIMEOUT次都还没等待到事件则认为通讯失败， 调用的SPI_TIMEOUT_UserCallback输出调试信息，并退出通讯；

3. 通过检测TXE标志，获取发送缓冲区的状态，若发送缓冲区为空， 则表示可能存在的上一个数据已经发送完毕；

4. 等待至发送缓冲区为空后，调用库函数SPI_I2S_SendData把要发送的数据“byte”写入到SPI的数据寄存器DR， 写入SPI数据寄存器的数据会存储到发送缓冲区，由SPI外设发送出去；

5. 写入完毕后等待RXNE事件，即接收缓冲区非空事件。 由于SPI双线全双工模式下MOSI与MISO数据传输是同步的(请对比“SPI通讯过程”阅读)，当接收缓冲区非空时， 表示上面的数据发送完毕，且接收缓冲区也收到新的数据；

6. 待至接收缓冲区非空时，通过调用库函数SPI_I2S_ReceiveData读取SPI的数据寄存器DR，就可以获取接收缓冲区中的新数据了。 代码中使用关键字“return”把接收到的这个数据作为SPI_FLASH_SendByte函数的返回值， 所以我们可以看到在下面定义的SPI接收数据函数SPI_FLASH_ReadByte，它只是简单地调用了SPI_FLASH_SendByte函数发送数据“Dummy_Byte”， 然后获取其返回值(因为不关注发送的数据，所以此时的输入参数“Dummy_Byte”可以为任意值)。 可以这样做的原因是SPI的接收过程和发送过程实质是一样的，收发同步进行，关键在于我们的上层应用中，关注的是发送还是接收的数据。

#### 2.2.5 FLASH基本操作函数（W25Q64)

##### 2.2.5.1 读取FLASH芯片ID

```c
// 读取FLASH ID
uint32_t SPI_FLASH_ReadID(void)
{
  uint32_t Temp = 0, Temp_H = 0, Temp_M = 0, Temp_L = 0;
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_JedecDeviceID);
  Temp_H = SPI_FLASH_SendByte(Dummy_Byte);
  Temp_M = SPI_FLASH_SendByte(Dummy_Byte);
  Temp_L = SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_CS_HIGH();
  Temp = (Temp_H << 16) | (Temp_M << 8) | Temp_L;
  return Temp;
}
```

这段代码利用控制CS引脚电平的宏“SPI_FLASH_CS_LOW/HIGH”以及前面编写的单字节收发函数SPI_FLASH_SendByte， 很清晰地实现了“JEDEC ID”指令的时序：发送一个字节的指令编码“W25X_JedecDeviceID”，然后读取3个字节， 获取FLASH芯片对该指令的响应，最后把读取到的这3个数据合并到一个变量Temp中，然后作为函数返回值， 把该返回值与我们定义的宏“sFLASH_ID”对比，即可知道FLASH芯片是否正常。

##### 2.2.5.2 读取FLASH Device ID

```c
// 读取FLASH Device ID
uint32_t SPI_FLASH_ReadDeviceID(void)
{
  uint32_t Temp = 0;
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_DeviceID);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  Temp = SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_CS_HIGH();
  return Temp;
}
```

##### 2.2.5.3 FLASH写使能以及读取当前状态

在向FLASH芯片存储矩阵写入数据前，首先要使能写操作，通过“Write Enable”命令即可写使能

```c
// 读取FLASH的状态寄存器
void SPI_FLASH_ReadStatusReg(uint32_t ReadAddr)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ReadData);
  // 发送地址的高、中、低位
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

// FALSH写入数据前，需要发送写使能命令
void SPI_FLASH_WriteEnable(void)
{
  SPI_FLASH_CS_LOW(); // 通讯开始，拉低CS
  SPI_FLASH_SendByte(W25X_WriteEnable);
  SPI_FLASH_CS_HIGH(); // 通讯结束，拉高CS
}
```

与EEPROM一样，由于FLASH芯片向内部存储矩阵写入数据需要消耗一定的时间，并不是在总线通讯结束的一瞬间完成的， 所以在写操作后需要确认FLASH芯片“空闲”时才能进行再次写入。为了表示自己的工作状态，FLASH芯片定义了一个状态寄存器

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI010.jpeg" title="" alt="" width="897">

我们只关注这个状态寄存器的第0位“BUSY”，当这个位为“1”时，表明FLASH芯片处于忙碌状态， 它可能正在对内部的存储矩阵进行“擦除”或“数据写入”的操作。

利用指令表中的“Read Status Register”指令可以获取FLASH芯片状态寄存器的内容， 其时序见图

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI011.jpeg" title="" alt="" width="893">

只要向FLASH芯片发送了读状态寄存器的指令，FLASH芯片就会持续向主机返回最新的状态寄存器内容， 直到收到SPI通讯的停止信号。据此我们编写了具有等待FLASH芯片写入结束功能的函数

##### 2.2.5.4 等待FLASH写入结束函数

```c
// 等待FLASH写入操作结束，即busy标志位为0
void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t status = 0;
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ReadStatusReg); // 发送读取状态寄存器命令
  do{
    status = SPI_FLASH_SendByte(Dummy_Byte); // 读取FLASH的状态寄存器
  }
  while((status & 0x01) == SET); // 等待WTP位清零
  SPI_FLASH_CS_HIGH();
}
```

这段代码发送读状态寄存器的指令编码“W25X_ReadStatusReg”后，在while循环里持续获取寄存器的内容并检验它的“WIP_Flag标志”(即BUSY位)， 一直等待到该标志表示写入结束时才退出本函数，以便继续后面与FLASH芯片的数据通讯。

##### 2.2.5.5 FLASH扇区删除

由于FLASH存储器的特性决定了它只能把原来为“1”的数据位改写成“0”，而原来为“0”的数据位不能直接改写为“1”。 所以这里涉及到数据“擦除”的概念，在写入前，必须要对目标存储矩阵进行擦除操作，把矩阵中的数据位擦除为“1”， 在数据写入的时候，如果要存储数据“1”，那就不修改存储矩阵，在要存储数据“0”时，才更改该位。

通常，对存储矩阵擦除的基本操作单位都是多个字节进行，如本例子中的FLASH芯片支持“扇区擦除”、“块擦除”以及“整片擦除”

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI06.png)

FLASH芯片的最小擦除单位为扇区(Sector)，而一个块(Block)包含16个扇区， 其内部存储矩阵分布见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI012.png)

使用扇区擦除指令可控制FLASH芯片开始擦写， 其指令时序见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI013.png)

扇区擦除指令的第一个字节为指令编码，紧接着发送的3个字节用于表示要擦除的存储矩阵地址。要注意的是在扇区擦除指令前， 还需要先发送“写使能”指令，发送扇区擦除指令后，通过读取寄存器状态等待扇区擦除操作完毕

```c
/ 擦除FLASH扇区，函数参数：扇区地址
void SPI_FLASH_SectorErase(uint32_t SectorAddress)
{
  SPI_FLASH_WriteEnable(); // 发送写使能命令
  SPI_FLASH_WaitForWriteEnd(); // 等待FLASH写入结束
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_SectorErase); // 发送扇区擦除命令
  // 发送扇区地址的高、中、低位
  SPI_FLASH_SendByte((SectorAddress & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((SectorAddress & 0xFF00) >> 8);
  SPI_FLASH_SendByte(SectorAddress & 0xFF);
  SPI_FLASH_CS_HIGH();
  SPI_FLASH_WaitForWriteEnd(); // 等待FLASH写入结束
}
```

这段代码调用的函数在前面都已讲解，只要注意发送擦除地址时高位在前即可。调用扇区擦除指令时注意输入的地址要对齐到4KB。

##### 2.2.5.7 整片删除FLASH

```c
// 整片擦除FLASH
void SPI_FLASH_BulkErase(void)
{
  SPI_FLASH_WriteEnable(); // 发送写使能命令
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ChipErase); // 发送整片擦除命令
  SPI_FLASH_CS_HIGH();
  SPI_FLASH_WaitForWriteEnd(); // 等待FLASH写入结束
}
```

##### 2.2.5.8 FLASH的页写入

目标扇区被擦除完毕后，就可以向它写入数据了。与EEPROM类似，FLASH芯片也有页写入命令， 使用页写入命令最多可以一次向FLASH传输256个字节的数据，我们把这个单位为页大小。 FLASH页写入的时序见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI014.jpeg)

从时序图可知，第1个字节为“页写入指令”编码，2-4字节为要写入的“地址A”，接着的是要写入的内容，最多个可以发送256字节数据， 这些数据将会从“地址A”开始，按顺序写入到FLASH的存储矩阵。若发送的数据超出256个，则会覆盖前面发送的数据。

与擦除指令不一样，页写入指令的地址并不要求按256字节对齐，只要确认目标存储单元是擦除状态即可(即被擦除后没有被写入过)。 所以，若对“地址x”执行页写入指令后，发送了200个字节数据后终止通讯，下一次再执行页写入指令，从“地址(x+200)”开始写入200个字节也是没有问题的(小于256均可)。 只是在实际应用中由于基本擦除单元是4KB，一般都以扇区为单位进行读写，把页写入时序封装成函数：

```c
// 对FLASH按页写入数据
// 函数参数：待写入数据指针、写入地址、写入长度
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  SPI_FLASH_WriteEnable(); // 发送写使能命令
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_PageProgram); // 发送页写入命令
  // 分别写入高、中、低位地址
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  SPI_FLASH_SendByte(WriteAddr & 0xFF);
  // 判断压迫写入的数据是否大于单页最大长度(256)
  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
    NumByteToWrite = SPI_FLASH_PerWritePageSize; // 截取写入长度
    FLASH_ERROR("FLASH写入数据长度超过单页最大长度！\r\n");
  }
  while(NumByteToWrite--)
  {
    SPI_FLASH_SendByte(*pBuffer); // 写入数据
    pBuffer++;
  }
  SPI_FLASH_CS_HIGH();
  SPI_FLASH_WaitForWriteEnd(); // 等待FLASH写入结束
}
```

这段代码的内容为：先发送“写使能”命令，接着才开始页写入时序，然后发送指令编码、地址，再把要写入的数据一个接一个地发送出去， 发送完后结束通讯，检查FLASH状态寄存器，等待FLASH内部写入结束。

##### 2.2.5.9 不定量数据写入

应用的时候我们常常要写入不定量的数据，直接调用“页写入”函数并不是特别方便，所以我们在它的基础上编写了“不定量数据写入”的函数

```c
// 对FLASH任意写入数据
// 函数参数：待写入数据指针、写入地址、写入长度
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  uint8_t NumOfPage = 0; // 定义页面数量
  uint8_t NumOfSingle = 0; // 定义单页写入数据长度
  uint8_t Addr = 0; // 定义地址偏移量
  uint8_t count = 0; // 计数器
  uint8_t temp = 0; // 临时变量
    // 1.mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0
  Addr = WriteAddr % SPI_FLASH_PageSize;
    // 2.差count个数据值，刚好可以对齐到页地址
  count = SPI_FLASH_PageSize - Addr;
    // 3.计算出要写多少整数页
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
    // 4.mod运算求余，计算出剩余不满一页的字节数
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
    // 若Addr=0,则WriteAddr 刚好按页对齐，直接写
  if (Addr == 0)
  {
        /* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0)
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    { 
            /*先把整数页都写了*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize); // 写入数据
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
            /*若有多余的不满一页的数据，把它写完*/
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle); // 写入剩余数据
    }
  }
    /* 若地址与 SPI_FLASH_PageSize 不对齐  */
  else 
  {
        /* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0)
    {
            /*当前页剩余的count个位置比NumOfSingle小，一页写不完*/
      if (NumOfSingle > count) 
      {
        temp = NumOfSingle - count;
                /*先写满当前页*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;
                /*再写剩余的数据*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else /*当前页剩余的count个位置能写完NumOfSingle个数据*/
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
            /*地址不对齐多出的count分开处理，不加入这个运算*/
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
            /* 先写完count个数据，为的是让下一次要写的地址对齐 */
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
            /* 接下来就重复地址对齐的情况 */
      WriteAddr +=  count;
      pBuffer += count;
            /*把整数页都写了*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
            /*若有多余的不满一页的数据，把它写完*/
      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}
```

这段代码与EEPROM章节中的“快速写入多字节”函数原理是一样的，运算过程在此不再赘述。区别是页的大小以及实际数据写入的时候， 使用的是针对FLASH芯片的页写入函数，且在实际调用这个“不定量数据写入”函数时，还要注意确保目标扇区处于擦除状态。

这个函数我们简单理解就行了，学会调用才是王道，不过我们也可以稍微解释一下使该教程更加丰富：

1. **变量定义：**
   
   - `NumOfPage`：要写入的完整页数。
   - `NumOfSingle`：页外剩余的字节数。
   - `Addr`：写入地址在页内的偏移量。
   - `count`：当前页剩余的空间。
   - `temp`：临时变量，用于计算和处理页外的剩余数据。

2. **计算页对齐和剩余空间：**
   
   - `Addr`：通过`WriteAddr % SPI_FLASH_PageSize`计算，表示写入地址在当前页的偏移量。
   - `count`：当前页剩余的空间，`SPI_FLASH_PageSize - Addr`。
   - `NumOfPage`：可以写入的完整页数，`NumByteToWrite / SPI_FLASH_PageSize`。
   - `NumOfSingle`：剩余的字节数，不足一页的部分，`NumByteToWrite % SPI_FLASH_PageSize`。

3. **处理页对齐的情况：**
   
   - 如果`Addr == 0`（地址对齐）：
     - 若数据小于一页（`NumOfPage == 0`），直接写入。
     - 若数据大于一页：
       - 先写入完整的页。
       - 然后处理剩余不足一页的数据。

4. **处理页不对齐的情况：**
   
   - 如果`Addr != 0`（地址不对齐）：
     - 如果数据小于一页：
       - 如果剩余空间不足以容纳剩余数据，先写满当前页，再写剩余的数据。
       - 否则直接写入。
     - 如果数据大于一页：
       - 先处理当前页剩余的空间（`count`）。
       - 写入完整的页数据。
       - 处理剩余的数据。

5. **写入函数 `SPI_FLASH_PageWrite`**：
   
   - 此函数负责实际写入数据到SPI Flash中，每次写入一页数据或少于一页的数据。

##### 2.2.5.10 从FLASH读取数据

相对于写入，FLASH芯片的数据读取要简单得多，使用读取指令“Read Data”即可， 其指令时序见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI015.jpeg)

发送了指令编码及要读的起始地址后，FLASH芯片就会按地址递增的方式返回存储矩阵的内容，读取的数据量没有限制， 只要没有停止通讯，FLASH芯片就会一直返回数据

```c
// 读取FLASH数据
// 函数参数：读取数据指针、读取地址、读取长度
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ReadData); // 发送读取数据命令
  // 发送地址的高、中、低位
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
  while(NumByteToRead--)
  {
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte); // 读取数据
    pBuffer++;
  }
  SPI_FLASH_CS_HIGH();
}
```

由于读取的数据量没有限制，所以发送读命令后一直接收NumByteToRead个数据到结束即可。

##### 2.2.5.11 进入掉电模式、唤醒FLASH

```c
// 进入掉电模式
void SPI_FLASH_PowerDowm(void)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_PowerDown); // 发送掉电命令
  SPI_FLASH_CS_HIGH();
}

// 唤醒FLASH
void SPI_FLASH_WakeUp(void)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ReleasePowerDown); // 发送唤醒命令
  SPI_FLASH_CS_HIGH();
}
```

#### 2.2.6 主函数

```c
#include "stm32f10x.h"
#include "led.h"
#include "spi_flash.h"
#include "usart.h"

// 定义测试用变量
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

// 获取缓冲区的长度
#define countof(a) (sizeof(a)/sizeof(*(a)))
#define TxBufferSize1 (countof(TxBuffer1)-1)
#define RxBufferSize1 (countof(RxBuffer1)-1)
#define BufferSize (countof(Tx_Buffer-1))

// 配置SPI FLASH 相关参数
#define FLASH_WriteAddress 0x00000 // 写地址
#define FLASH_ReadAddress FLASH_WriteAddress   // 读地址
#define FLASH_SectorToErase FLASH_WriteAddress // 扇区擦除地址

// 发送缓冲区初始化
uint8_t Tx_Buffer[] = "Hello";
uint8_t Rx_Buffer[BufferSize];

__IO uint32_t DeviceID = 0; // 设备ID
__IO uint32_t FlashID = 0; // FLASH ID
__IO TestStatus TransferStatus1 = FAILED; // 测试结果

// 比较两个缓冲区数据是否相等
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
    while(BufferLength--)
    {
        if(*pBuffer1!=*pBuffer2)
        {
            return FAILED;
        }
        pBuffer1++;
        pBuffer2++;
    }
    return PASSED;
}

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

int main(void)
{
    LED_Init();
    LED_BLUE();
    USART_Config();
    SPI_FLASH_Init();
    DeviceID = SPI_FLASH_ReadDeviceID(); // 读取设备ID
    Delay(200);
    FlashID = SPI_FLASH_ReadID(); // 读取FLASH ID
    Delay(200);
    printf("DeviceID = 0x%08X\r\n", DeviceID);
    printf("FlashID = 0x%08X\r\n", FlashID);
    if(FlashID == sFLASH_ID)
    {
        printf("\r\n 检测到flash W25Q64 \r\n");
        SPI_FLASH_SectorErase(FLASH_SectorToErase); // 擦除扇区
        SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize); // 写数据
        printf("\r\n 写入的数据为：%s \r\n", Tx_Buffer);
        Delay(200);
        SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize); // 读数据
        printf("\r\n 读出的数据为：%s \r\n", Rx_Buffer);
        Delay(200);
        TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize); // 比较读写数据是否一致
        if(TransferStatus1 == PASSED)
        {
            LED_GREEN();
            printf("\r\n 读写测试成功 \r\n");
        }
        else
        {
            LED_RED();
            printf("\r\n 读写测试失败 \r\n");
        }
    }
    else
    {
        LED_YELLOW();
        printf("\r\n 未检测到flash W25Q64 \r\n");
    }
    while(1)
    {
    }
}
```

函数中初始化了LED、串口、SPI外设，然后读取FLASH芯片的ID进行校验，若ID校验通过则向FLASH的特定地址写入测试数据， 然后再从该地址读取数据，测试读写是否正常。

## 3. 小结

总得来说这一章不是很难，就是一个编写驱动函数的过程，在小结里，我们简化整个过程再来回顾一下：

1. **初始化SPI接口**：
   
   - 配置SPI外设的工作模式、数据宽度、时钟极性和相位等。
   - 启动SPI外设。

2. **发送和接收SPI数据**：
   
   - 使用STM32的SPI库函数或直接访问寄存器来进行数据传输。

3. **控制Flash操作**：
   
   - 根据Flash存储器的指令集执行读、写、擦除等操作。

以下是一个简单的示例代码，展示了如何在STM32上使用SPI接口与一个典型的SPI Flash存储器进行通信。这些操作包括读取Flash ID、写入数据、读取数据等。

### 3.1 初始化SPI接口

```c
#include "stm32f4xx_hal.h"

SPI_HandleTypeDef hspi1;

void MX_SPI1_Init(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    HAL_SPI_Init(&hspi1);
}
```

### 3.2 SPI Flash命令

以下是与SPI Flash进行通信的一些常见命令：

- 读数据命令：`0x03`
- 写数据命令：`0x02`
- 读取ID命令：`0x9F`

### 3.3 读Flash ID

```c
uint8_t SPI_FLASH_ReadID(void)
{
    uint8_t id[3];
    uint8_t cmd = 0x9F; // 读取ID命令

    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, id, 3, HAL_MAX_DELAY);

    return id[0]; // 返回ID的第一个字节
}
```

### 3.4 写入数据到Flash

```c
void SPI_FLASH_WriteData(uint32_t addr, uint8_t *pData, uint16_t size)
{
    uint8_t cmd[4];
    cmd[0] = 0x02; // 写数据命令
    cmd[1] = (addr >> 16) & 0xFF;
    cmd[2] = (addr >> 8) & 0xFF;
    cmd[3] = addr & 0xFF;

    HAL_SPI_Transmit(&hspi1, cmd, 4, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi1, pData, size, HAL_MAX_DELAY);
}
```

### 3.5 读取数据从Flash

```c
void SPI_FLASH_ReadData(uint32_t addr, uint8_t *pData, uint16_t size)
{
    uint8_t cmd[4];
    cmd[0] = 0x03; // 读数据命令
    cmd[1] = (addr >> 16) & 0xFF;
    cmd[2] = (addr >> 8) & 0xFF;
    cmd[3] = addr & 0xFF;

    HAL_SPI_Transmit(&hspi1, cmd, 4, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, pData, size, HAL_MAX_DELAY);
}
```

### 3.6 主程序示例

```c
int main(void)
{
    HAL_Init();
    MX_SPI1_Init();

    uint8_t id = SPI_FLASH_ReadID();
    // 处理ID

    uint8_t writeData[256] = {0};
    uint8_t readData[256] = {0};

    // 写入数据
    SPI_FLASH_WriteData(0x000000, writeData, sizeof(writeData));

    // 读取数据
    SPI_FLASH_ReadData(0x000000, readData, sizeof(readData));

    while (1)
    {
        // 主循环
    }
}
```

---

2024.9.5 第一次修订，后期不再维护

2025.1.21 程序和内容整体大修，方便阅读
