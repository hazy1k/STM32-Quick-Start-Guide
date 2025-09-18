# 第十二章 DMA介绍

## 1. 导入

    DMA(Direct Memory Access)—直接存储器存取，是单片机的一个外设，它的主要功能是用来搬数据，但是不需要占用CPU， 即在传输数据的时候，CPU可以干其他的事情，好像是多线程一样。数据传输支持从外设到存储器或者存储器到存储器， 这里的存储器可以是SRAM或者是FLASH。DMA控制器包含了DMA1和DMA2，其中DMA1有7个通道，DMA2有5个通道， 这里的通道可以理解为传输数据的一种管道。

更多介绍参考：[STM32速成笔记—DMA - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/663191540)

## 2. DMA功能框图

    DMA控制器独立于内核，属于一个单独的外设，结构比较简单，从编程的角度来看，我们只需掌握功能框图中的三部分内容即可

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/DMA002.png)

### 2.1 DMA请求

如果外设要想通过DMA来传输数据，必须先给DMA控制器发送DMA请求，DMA收到请求信号之后，控制器会给外设一个应答信号， 当外设应答后且DMA控制器收到应答信号之后，就会启动DMA的传输，直到传输完毕。

DMA有DMA1和DMA2两个控制器，DMA1有7个通道，DMA2有5个通道，不同的DMA控制器的通道对应着不同的外设请求， 这决定了我们在软件编程上该怎么设置，具体见DMA请求映像表。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/DMA003.png)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/DMA004.png)

其中ADC3、SDIO和TIM8的DMA请求只在大容量产品中存在，这个在具体项目时要注意。

### 2.2 通道

DMA具有12个独立可编程的通道，其中DMA1有7个通道，DMA2有5个通道，每个通道对应不同的外设的DMA请求。 虽然每个通道可以接收多个外设的请求，但是同一时间只能接收一个，不能同时接收多个。

### 2.3 仲裁器

当发生多个DMA通道请求时，就意味着有先后响应处理的顺序问题，这个就由仲裁器也管理。仲裁器管理DMA通道请求分为两个阶段。 第一阶段属于软件阶段，可以在DMA_CCRx寄存器中设置，有4个等级：非常高、高、中和低四个优先级。第二阶段属于硬件阶段， 如果两个或以上的DMA通道请求设置的优先级一样，则他们优先级取决于通道编号，编号越低优先权越高，比如通道0高于通道1。 在大容量产品和互联型产品中， DMA1控制器拥有高于DMA2控制器的优先级。

## 3. DMA数据配置

使用DMA，最核心就是配置要传输的数据，包括数据从哪里来，要到哪里去，传输的数据的单位是什么，要传多少数据，是一次传输还是循环传输等等。

### 3.1 从哪里来到哪里去

我们知道DMA传输数据的方向有三个：从外设到存储器，从存储器到外设，从存储器到存储器。 具体的方向DMA_CCR位4 DIR配置：0表示从外设到存储器，1表示从存储器到外设。 这里面涉及到的外设地址由DMA_CPAR配置，存储器地址由DMA_CMAR配置。

- 外设到存储器

当我们使用从外设到存储器传输时，以ADC采集为例。DMA外设寄存器的地址对应的就是ADC数据寄存器的地址， DMA存储器的地址就是我们自定义的变量（用来接收存储AD采集的数据）的地址。方向我们设置外设为源地址。

- 存储器到外设

当我们使用从存储器到外设传输时，以串口向电脑端发送数据为例。DMA外设寄存器的地址对应的就是串口数据寄存器的地址， DMA存储器的地址就是我们自定义的变量（相当于一个缓冲区，用来存储通过串口发送到电脑的数据）的地址。方向我们设置外设为目标地址。

- 存储器到存储器

当我们使用从存储器到存储器传输时，以内部FLASH向内部SRAM复制数据为例。 DMA外设寄存器的地址对应的就是内部FLASH（我们这里把内部FALSH当作一个外设来看）的地址， DMA存储器的地址就是我们自定义的变量（相当于一个缓冲区，用来存储来自内部FLASH的数据）的地址。 方向我们设置外设（即内部FLASH）为源地址。跟上面两个不一样的是，这里需要把DMA_CCR位14：MEM2MEM：存储器到存储器模式配置为1，启动M2M模式。

### 3.2 要传多少，单位是什么

当我们配置好数据要从哪里来到哪里去之后，我们还需要知道我们要传输的数据是多少，数据的单位是什么。

以串口向电脑发送数据为例，我们可以一次性给电脑发送很多数据，具体多少由DMA_CNDTR配置， 这是一个32位的寄存器，一次最多只能传输65535个数据。

要想数据传输正确，源和目标地址存储的数据宽度还必须一致，串口数据寄存器是8位的， 所以我们定义的要发送的数据也必须是8位。外设的数据宽度由DMA_CCRx的PSIZE[1:0]配置， 可以是8/16/32位，存储器的数据宽度由DMA_CCRx的MSIZE[1:0]配置，可以是8/16/32位。

在DMA控制器的控制下，数据要想有条不紊的从一个地方搬到另外一个地方，还必须正确设置两边数据指针的增量模式。 外设的地址指针由DMA_CCRx的PINC配置，存储器的地址指针由MINC配置。以串口向电脑发送数据为例，要发送的数据很多， 每发送完一个，那么存储器的地址指针就应该加1，而串口数据寄存器只有一个， 那么外设的地址指针就固定不变。具体的数据指针的增量模式由实际情况决定。

### 3.3 什么时候传输完成

数据什么时候传输完成，我们可以通过查询标志位或者通过中断的方式来鉴别。每个DMA通道在DMA传输过半、 传输完成和传输错误时都会有相应的标志位，如果使能了该类型的中断后，则会产生中断。有关各个标志位的详细描述请参考DMA中断状态寄存器DMA_ISR的详细描述。

传输完成还分两种模式，是一次传输还是循环传输，一次传输很好理解，即是传输一次之后就停止，要想再传输的话， 必须关断DMA使能后再重新配置后才能继续传输。循环传输则是一次传输完成之后又恢复第一次传输时的配置循环传输， 不断的重复。具体的由DMA_CCRx寄存器的CIRC 循环模式位控制。

## 4. DMA初始化结构体详解

标准库函数对每个外设都建立了一个初始化结构体xxx_InitTypeDef(xxx为外设名称)，结构体成员用于设置外设工作参数， 并由标准库函数xxx_Init()调用这些设定参数进入设置外设相应的寄存器，达到配置外设工作环境的目的。

结构体xxx_InitTypeDef和库函数xxx_Init配合使用是标准库精髓所在，理解了结构体xxx_InitTypeDef每个成员意义基本上就可以对该外设运用自如。 结构体xxx_InitTypeDef定义在stm32f10x_xxx.h(后面xxx为外设名称)文件中，库函数xxx_Init定义在stm32f10x_xxx.c文件中， 编程时我们可以结合这两个文件内注释使用。

```c
typedef struct
{
    uint32_t DMA_PeripheralBaseAddr;   // 外设地址
    uint32_t DMA_MemoryBaseAddr;       // 存储器地址
    uint32_t DMA_DIR;                  // 传输方向
    uint32_t DMA_BufferSize;           // 传输数目
    uint32_t DMA_PeripheralInc;        // 外设地址增量模式
    uint32_t DMA_MemoryInc;            // 存储器地址增量模式
    uint32_t DMA_PeripheralDataSize;   // 外设数据宽度
    uint32_t DMA_MemoryDataSize;       // 存储器数据宽度
    uint32_t DMA_Mode;                 // 模式选择
    uint32_t DMA_Priority;             // 通道优先级
    uint32_t DMA_M2M;                  // 存储器到存储器模式
} DMA_InitTypeDef;
```

1) DMA_PeripheralBaseAddr： 外设地址，设定DMA_CPAR寄存器的值；一般设置为外设的数据寄存器地址，如果是存储器到存储器模式则设置为其中一个存储器地址。

2) DMA_Memory0BaseAddr： 存储器地址，设定DMA_CMAR寄存器值；一般设置为我们自定义存储区的首地址。

3) DMA_DIR： 传输方向选择，可选外设到存储器、存储器到外设。它设定DMA_CCR寄存器的DIR[1:0]位的值。这里并没有存储器到存储器的方向选择，当使用存储器到存储器时，只需要把其中一个存储器当作外设使用即可。

4) DMA_BufferSize： 设定待传输数据数目，初始化设定DMA_CNDTR寄存器的值。

5) DMA_PeripheralInc： 如果配置为DMA_PeripheralInc_Enable，使能外设地址自动递增功能，它设定DMA_CCR寄存器的PINC位的值；一般外设都是只有一个数据寄存器，所以一般不会使能该位。

6) DMA_MemoryInc： 如果配置为DMA_MemoryInc_Enable，使能存储器地址自动递增功能，它设定DMA_CCR寄存器的MINC位的值；我们自定义的存储区一般都是存放多个数据的，所以要使能存储器地址自动递增功能。

7) DMA_PeripheralDataSize： 外设数据宽度，可选字节(8位)、半字(16位)和字(32位)，它设定DMA_CCR寄存器的PSIZE[1:0]位的值。

8) DMA_MemoryDataSize： 存储器数据宽度，可选字节(8位)、半字(16位)和字(32位)，它设定DMA_CCR寄存器的MSIZE[1:0]位的值。当外设和存储器之间传数据时，两边的数据宽度应该设置为一致大小。

9) DMA_Mode： DMA传输模式选择，可选一次传输或者循环传输，它设定DMA_CCR寄存器的CIRC位的值。例程我们的ADC采集是持续循环进行的，所以使用循环传输模式。

10) DMA_Priority： 软件设置通道的优先级，有4个可选优先级分别为非常高、高、中和低，它设定DMA_CCR寄存器的PL[1:0]位的值。DMA通道优先级只有在多个DMA通道同时使用时才有意义，如果是单个通道，优先级可以随便设置。

11) DMA_M2M： 存储器到存储器模式，使用存储器到存储器时用到，设定DMA_CCR的位14 MEN2MEN即可启动存储器到存储器模式。

### 4.1 简单举例

单单学了结构体怎么行，我们马上就来使用

下面是一个简单的例子，演示如何使用 `DMA_InitTypeDef` 结构体来配置一个DMA通道进行内存到外设的数据传输。这个例子将通过DMA将数据从内存传输到USART外设。

```c
#include "stm32f4xx.h"

// 定义缓冲区大小和数据
#define BUFFER_SIZE 64
uint8_t tx_buffer[BUFFER_SIZE] = "Hello, DMA with USART1!";

// 配置DMA
void DMA_Config(void) {
    DMA_InitTypeDef DMA_InitStruct;

    // 启用DMA1时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    // 配置DMA通道
    DMA_InitStruct.DMA_Channel = DMA_Channel_4; // 选择DMA通道4
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR)); // 外设基地址为USART1的数据寄存器
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)tx_buffer; // 内存基地址为数据缓冲区
    DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral; // 数据方向从内存到外设
    DMA_InitStruct.DMA_BufferSize = BUFFER_SIZE; // 设置传输缓冲区大小
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不递增
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址递增
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataAlign_Byte; // 外设数据宽度为字节
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataAlign_Byte; // 内存数据宽度为字节
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal; // DMA工作模式为正常模式
    DMA_InitStruct.DMA_Priority = DMA_Priority_High; // DMA通道优先级为高
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable; // 禁用FIFO模式
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; // FIFO阈值为满
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single; // 内存突发模式为单次
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; // 外设突发模式为单次
    // 初始化DMA2流7
    DMA_Init(DMA1_Stream4, &DMA_InitStruct);
    // 启用DMA传输完成中断
    DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);
    // 配置USART1使用DMA进行数据传输
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    // 启动DMA传输
    DMA_Cmd(DMA1_Stream4, ENABLE);
}

// 主函数
int main(void) {
    // 初始化系统时钟
    SystemInit();
    // 配置USART1
    USART_InitTypeDef USART_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // 启用USART1时钟
    USART_InitStruct.USART_BaudRate = 9600; // 设置波特率为9600
    USART_InitStruct.USART_WordLength = USART_WordLength_8b; // 8位数据长度
    USART_InitStruct.USART_StopBits = USART_StopBits_1; // 1位停止位
    USART_InitStruct.USART_Parity = USART_Parity_No; // 无奇偶校验
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 不使用硬件流控制
    USART_InitStruct.USART_Mode = USART_Mode_Tx; // 只配置为发送模式
    USART_Init(USART1, &USART_InitStruct); // 初始化USART1
    USART_Cmd(USART1, ENABLE); // 启用USART1
    // 配置DMA
    DMA_Config();
    // 主循环
    while (1) {
        // 主循环中可以添加其他代码
    }
}
```

### 4.2 详细注释

1. **DMA_Config 函数**:
   
   - **`DMA_InitStruct.DMA_Channel`**: 选择DMA通道4，适用于USART1。
   - **`DMA_InitStruct.DMA_PeripheralBaseAddr`**: 设置外设基地址为USART1的数据寄存器地址。
   - **`DMA_InitStruct.DMA_Memory0BaseAddr`**: 设置内存基地址为 `tx_buffer` 的起始地址。
   - **`DMA_InitStruct.DMA_DIR`**: 设置数据传输方向为从内存到外设。
   - **`DMA_InitStruct.DMA_BufferSize`**: 设置传输的数据量为缓冲区大小。
   - **`DMA_InitStruct.DMA_PeripheralInc`**: 禁用外设地址递增，因为USART寄存器地址固定。
   - **`DMA_InitStruct.DMA_MemoryInc`**: 启用内存地址递增。
   - **`DMA_InitStruct.DMA_PeripheralDataSize`** 和 **`DMA_InitStruct.DMA_MemoryDataSize`**: 设置数据宽度为字节。
   - **`DMA_InitStruct.DMA_Mode`**: 设置为正常模式。
   - **`DMA_InitStruct.DMA_Priority`**: 设置DMA通道优先级为高。
   - **`DMA_InitStruct.DMA_FIFOMode`**, **`DMA_InitStruct.DMA_FIFOThreshold`**, **`DMA_InitStruct.DMA_MemoryBurst`**, **`DMA_InitStruct.DMA_PeripheralBurst`**: 配置FIFO和突发模式。

2. **主函数**:
   
   - 初始化系统时钟。
   - 配置USART1的基本参数，并启用USART1。
   - 调用 `DMA_Config` 函数配置DMA。
   - 进入主循环，保持程序运行。

通过这种配置，你可以利用DMA将数据从内存高效地传输到USART外设进行发送。

---

2024.7.29 从后面起，就单纯讲理论原理了，实验单独开章节讲，因为把实验加上去整篇就显得过于长了，并且不利于分类阅读，以前的加了就加了吧，后面理论和实验分章节讲解

2024.8.28 第二次修订，后期不再维护

2024.9.4 有阅读量，加更
