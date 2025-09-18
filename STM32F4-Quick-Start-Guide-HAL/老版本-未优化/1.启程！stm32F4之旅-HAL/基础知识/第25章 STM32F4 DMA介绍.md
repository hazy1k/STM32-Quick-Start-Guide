# 第二十五章 STM32F4 DMA介绍

## 1. STM32F4 DMA简介

DMA，全称为： Direct Memory Access，即直接存储器访问。 DMA 传输方式无需 CPU 直接控制传输，也没有中断处理方式那样保留现场和恢复现场的过程，通过硬件为 RAM 与 I/O 设备开辟一条直接传送数据的通路， 能使 CPU 的效率大为提高。

STM32F4 最多有 2 个 DMA 控制器（DMA1 和 DMA2）， 共 16 个数据流（每个控制器 8 个）， 每一个 DMA 控制器都用于管理一个或多个外设的存储器访问请求。每个数据流总共可以有多达 8个通道（或称请求）。每个数据流通道都有一个仲裁器，用于处理 DMA 请求间的优先级。

STM32F4 有两个 DMA 控制器， DMA1 和 DMA2， 本章， 我们仅针对 DMA2 进行介绍。STM32F4 的 DMA 控制器框图如图所示：

![屏幕截图 2024-10-17 212207.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/17-21-22-14-屏幕截图%202024-10-17%20212207.png)

DMA 控制器执行直接存储器传输：因为采用 AHB 主总线，它可以控制 AHB 总线矩阵来启动 AHB 事务。它可以执行下列事务：

1. 外设到存储器的传输

2. 存储器到外设的传输

3. 存储器到存储器的传输

这里特别注意一下，存储器到存储器需要外设接口可以访问存储器，而仅 DMA2 的外设接口可以访问存储器，所以仅 DMA2 控制器支持存储器到存储器的传输， DMA1 不支持。

数据流的多通道选择，是通过 DMA_SxCR 寄存器控制的，如图所示：

![屏幕截图 2024-10-17 212347.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/17-21-23-52-屏幕截图%202024-10-17%20212347.png)

从上图可以看出， DMA_SxCR 控制数据流到底使用哪一个通道，每个数据流有 8 个通道可供选择，每次只能选择其中一个通道进行 DMA 传输。接下来，我们看看 DMA2 的各数据流通道映射表

![屏幕截图 2024-10-17 212426.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/17-21-24-29-屏幕截图%202024-10-17%20212426.png)

上表就列出了 DMA2 所有可能的选择情况，来总共 64 种组合，比如本章我们要实现串口 1的 DMA 发送，即 USART1_TX，就必须选择 DMA2 的数据流 7，通道 4，来进行 DMA 传输。这里注意一下，有的外设（比如 USART1_RX）可能有多个通道可以选择，大家随意选择一个就可以了。、

## 2. 相关寄存器介绍

### 2.1 中断状态寄存器（DMA_LISR和DMA_HISR）

第一个是 DMA 中断状态寄存器，该寄存器总共有 2 个： DMA_LISR 和 DMA_HISR，每个寄存器管理 4 数据流（总共 8 个）， DMA_LISR 寄存器用于管理数据流 0~3，而 DMA_HISR 用于管理数据流 4~7。这两个寄存器各位描述都完全一模一样，只是管理的数据流不一样。

这里，我们仅以 DMA_LISR 寄存器为例进行介绍， DMA_LISR 各位描述如图所示：

![屏幕截图 2024-10-17 212637.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/17-21-27-07-屏幕截图%202024-10-17%20212637.png)

如果开启了 DMA_LISR 中这些位对应的中断， 则在达到条件后就会跳到中断服务函数里面去，即使没开启，我们也可以通过查询这些位来获得当前 DMA 传输的状态。这里我们常用的是 TCIFx位，即数据流 x 的 DMA 传输完成与否标志。注意此寄存器为只读寄存器，所以在这些位被置位之后，只能通过其他的操作来清除。 DMA_HISR 寄存器各位描述通 DMA_LISR 寄存器各位描述完全一样，只是对应数据流 4~7，这里我们就不列出来了。

**注意**：每个通道都有三个标志位：

- `TCIF`：传输完成标志
- `HTIF`：半传输完成标志
- `TEIF`：传输错误标志

以下是一个简单的 ISR 示例，显示如何检查和清除这些标志位：

```c
void DMA2_Stream0_IRQHandler(void) {
    // 检查 DMA2 流 0 的状态
    if (__HAL_DMA_GET_FLAG(&hdma_adc1, DMA_FLAG_TC0)) {
        // 处理传输完成
        __HAL_DMA_CLEAR_FLAG(&hdma_adc1, DMA_FLAG_TC0); // 清除传输完成标志
        // 在这里处理接收到的数据
    }

    if (__HAL_DMA_GET_FLAG(&hdma_adc1, DMA_FLAG_HT0)) {
        // 处理半传输完成
        __HAL_DMA_CLEAR_FLAG(&hdma_adc1, DMA_FLAG_HT0); // 清除半传输完成标志
        // 在这里可以处理部分数据
    }

    if (__HAL_DMA_GET_FLAG(&hdma_adc1, DMA_FLAG_TE0)) {
        // 处理传输错误
        __HAL_DMA_CLEAR_FLAG(&hdma_adc1, DMA_FLAG_TE0); // 清除传输错误标志
        // 进行错误处理
    }
}
```

### 2.2 中断标志清除寄存器（DMA_LIFCR 和 DMA_HIFCR）

该寄存器同样有 2 个： DMA_LIFCR 和 DMA_HIFCR，同样是每个寄存器控制 4 个数据流， DMA_LIFCR 寄存器用于管理数据流 0~3，而 DMA_ HIFCR 用于管理数据流 4~7。这两个寄存器各位描述都完全一模一样，只是管理的数据流不一样。

这里，我们仅以 DMA_LIFCR 寄存器为例进行介绍， DMA_LIFCR 各位描述如图所示：

![屏幕截图 2024-10-17 213505.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/17-21-35-08-屏幕截图%202024-10-17%20213505.png)

DMA_LIFCR 的各位就是用来清除 DMA_LISR 的对应位的，通过写 1 清除。在 DMA_LISR 被置位后，我们必须通过向该位寄存器对应的位写入 1 来清除。 DMA_HIFCR 的使用同 DMA_LIFCR 类似，这里就不做介绍了。

以下是一个简单的 ISR 示例，显示如何检查和清除中断标志：

```c
void DMA2_Stream0_IRQHandler(void) {
    // 检查 DMA2 流 0 的状态
    if (__HAL_DMA_GET_FLAG(&hdma_adc1, DMA_FLAG_TC0)) {
        // 处理传输完成
        __HAL_DMA_CLEAR_FLAG(&hdma_adc1, DMA_FLAG_TC0); // 清除状态标志
        DMA2->LIFCR |= DMA_LIFCR_CTCIF0; // 清除传输完成标志
        // 在这里处理接收到的数据
    }

    if (__HAL_DMA_GET_FLAG(&hdma_adc1, DMA_FLAG_HT0)) {
        // 处理半传输完成
        __HAL_DMA_CLEAR_FLAG(&hdma_adc1, DMA_FLAG_HT0); // 清除状态标志
        DMA2->LIFCR |= DMA_LIFCR_CHTIF0; // 清除半传输完成标志
        // 在这里可以处理部分数据
    }

    if (__HAL_DMA_GET_FLAG(&hdma_adc1, DMA_FLAG_TE0)) {
        // 处理传输错误
        __HAL_DMA_CLEAR_FLAG(&hdma_adc1, DMA_FLAG_TE0); // 清除状态标志
        DMA2->LIFCR |= DMA_LIFCR_CTEIF0; // 清除传输错误标志
        // 进行错误处理
    }
}
```

### 2.3 数据流x配置寄存器（DMA_SxCR）（x=0~7）

配置 DMA_SxCR 寄存器时，可以根据以下步骤进行设置：

1. **选择 DMA 通道**：设置 `CHSEL` 位以选择相应的 DMA 通道。
2. **设置数据方向**：配置 `DIR` 位以确定数据传输的方向。
3. **选择传输大小**：配置数据传输的大小（通常通过其他寄存器进行设置）。
4. **配置优先级**：使用 `PL` 位设置 DMA 的优先级。
5. **启用中断**：根据需要设置相应的中断使能位（`TEIE`、`HTIE`、`TCIE`）。
6. **启用 DMA**：最后，将 `EN` 位设置为 1 以使能数据流。

以下是一个示例代码，展示如何配置 DMA_SxCR 寄存器以进行 ADC 数据传输：

```c
#include "stm32f4xx_hal.h"

// 假设我们使用 DMA1 流 0，配置为从 ADC 外设读取数据到内存
void DMA_Config(void) {
    DMA_HandleTypeDef hdma_adc;
    // DMA 控制器时钟使能
    __HAL_RCC_DMA1_CLK_ENABLE();
    // 配置 DMA 流参数
    hdma_adc.Instance = DMA1_Stream0; // 使用 DMA1 流 0
    hdma_adc.Init.Channel = DMA_CHANNEL_0; // 选择 DMA 通道
    hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY; // 从外设到内存
    hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不增量
    hdma_adc.Init.MemInc = DMA_MINC_ENABLE; // 内存地址增量
    hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; // 外设数据对齐
    hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_WORD; // 内存数据对齐
    hdma_adc.Init.Mode = DMA_CIRCULAR; // 循环模式
    hdma_adc.Init.Priority = DMA_PRIORITY_HIGH; // 高优先级
    hdma_adc.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // 禁用 FIFO
    // 初始化 DMA
    if (HAL_DMA_Init(&hdma_adc) != HAL_OK) {
        // 处理错误
    }
    // 使能 DMA 流
    __HAL_DMA_ENABLE(&hdma_adc);
}
// 在主程序中调用配置函数
int main(void) {
    HAL_Init(); // HAL 库初始化
    DMA_Config(); // 配置 DMA
    // 其他代码...
}
```

### 2.4 数据流x数据项数寄存器（DMA_SxNDTR）

这个寄存器控制 DMA 数据流 x 的每次传输所要传输的数据量。其设置范围为 0~65535。并且该寄存器的值会随着传输的进行而减少，当该寄存器的值为 0 的时候就代表此次数据传输已经全部发送完成了。所以可以通过这个寄存器的值来知道当前 DMA 传输的进度。 特别注意，这里是数据项数目，而不是指的字节数。比如设置数据位宽为 16 位，那么传输一次（一个项）就是 2 个字节。

### 2.5 数据流x的外设地址寄存器（DMA_SxPAR）

该寄存器用来存储 STM32F4 外设的地址，比如我们使用串口 1，那么该寄存器必须写入 0x40011004（其实就是&USART1_DR）。如果使用其他外设，就修改成相应外设的地址就行了。

### 2.6 数据流x的存储器地址寄存器

由于 STM32F4 的 DMA 支持双缓存，所以存储器地址寄存器有两个： DMA_SxM0AR 和 DMA_SxM1AR，其中 DMA_SxM1AR 仅在双缓冲模式下，才有效。本章我们没用到双缓冲模式，所以存储器地址寄存器就是： DMA_SxM0AR， 该寄存器和DMA_CPARx 差不多，但是是用来放存储器的地址的。比如我们使用 SendBuf[8200]数组来做存储器，那么我们在 DMA_SxM0AR 中写入&SendBuff 就可以了。

## 3. 配置DMA基本步骤

### 3.1 使能DMA2时钟，并等待数据流可配置

DMA 的时钟使能是通过 AHB1ENR 寄存器来控制的，这里我们要先使能时钟，才可以配置 DMA相关寄存器。 HAL 库方法为：

```c
__HAL_RCC_DMA2_CLK_ENABLE();//DMA2 时钟使能
__HAL_RCC_DMA1_CLK_ENABLE();//DMA1 时钟使能
```

### 3.2 初始化DMA2数据流

DMA 的某个数据流各种配置参数初始化是通过 HAL_DMA_Init 函数实现的，该函数声明为：

```c
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma);
```

该函数只有一个 DMA_HandleTypeDef 结构体指针类型入口参数，结构体定义为：

```c
typedef struct __DMA_HandleTypeDef
{
    DMA_Stream_TypeDef *Instance;
    DMA_InitTypeDef Init;
    HAL_LockTypeDef Lock;
    __IO HAL_DMA_StateTypeDef State;
    void *Parent;
    void (*XferCpltCallback)(struct __DMA_HandleTypeDef * hdma);
    void (*XferHalfCpltCallback)(struct __DMA_HandleTypeDef * hdma);
    void (*XferM1CpltCallback)(struct __DMA_HandleTypeDef * hdma);
    void (*XferErrorCallback)(struct __DMA_HandleTypeDef * hdma);
    __IO uint32_t ErrorCode;
    uint32_t StreamBaseAddress;
    uint32_t StreamIndex;
}DMA_HandleTypeDef;
```

成员变量 Instance 是用来设置寄存器基地址，例如要设置为 DMA2 的数据流 7，那么取值为 DMA2_Stream7。

成员变量 Parent 是 HAL 库处理中间变量，用来指向 DMA 通道外设句柄。

成员变量 XferCpltCallback（传输完成回调函数） , XferHalfCpltCallback（半传输完成回调函数） , XferM1CpltCallback（ Memory1 传输完成回调函数）和 XferErrorCallback（传输错误回调函数）是四个函数指针，用来指向回调函数入口地址。

成员变量 StreamBaseAddress 和 StreamIndex 是数据流基地址和索引号，这个是 HAL 库处理的时候会自动计算，用户无需设置。

其他成员变量 HAL 库处理过程状态标识变量，这里就不做过多讲解。接下来我们着重看看成员变量 Init，它是 DMA_InitTypeDef 结构体类型，该结构体定义为：

```c
typedef struct
{
    uint32_t Channel;  // 通道，例如： DMA_CHANNEL_4
    uint32_t Direction;// 传输方向，例如存储器到外设 DMA_MEMORY_TO_PERIPH
    uint32_t PeriphInc;// 外设（非）增量模式，非增量模式 DMA_PINC_DISABLE
    uint32_t MemInc;   // 存储器（非）增量模式，增量模式 DMA_MINC_ENABLE
    uint32_t PeriphDataAlignment;// 外设数据大小： 8/16/32 位。
    uint32_t MemDataAlignment;   // 存储器数据大小： 8/16/32 位。
    uint32_t Mode;               // 模式：外设流控模式/循环模式/普通模式
    uint32_t Priority;           // DMA 优先级：低/中/高/非常高
    uint32_t FIFOMode;           // FIFO 模式开启或者禁止
    uint32_t FIFOThreshold;      // FIFO 阈值选择：
    uint32_t MemBurst;           // 存储器突发模式：单次/4 个节拍/8 个节拍/16 个节拍
    uint32_t PeriphBurst;        // 外设突发模式：单次/4 个节拍/8 个节拍/16 个节拍
}DMA_InitTypeDef;
```

该结构体成员变量非常多，但是每个成员变量配置的基本都是 DMA_SxCR 寄存器和DMA_SxFCR 寄存器的相应位。我们把结构体各个成员变量的含义都通过注释的方式列出来了。例如本实验我们要用到 DMA2_Stream7 的 DMA_CHANNEL_4，把内存中数组的值发送到串口外设发送寄存器 DR，所以方向为存储器到外设 DMA_MEMORY_TO_PERIPH，一个一个字节发送，需要数字索引自动增加，所以是存储器增量模式 DMA_MINC_ENABLE，存储器和外设的字宽都是字节 8 位。具体配置如下：

```c
DMA_HandleTypeDef UART1TxDMA_Handler; //DMA 句柄
UART1TxDMA_Handler.Instance= DMA2_Stream7; //数据流选择
UART1TxDMA_Handler.Init.Channel=DMA_CHANNEL_4; //通道选择
UART1TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH; //存储器到外设
UART1TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE; //外设非增量模式
UART1TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE; //存储器增量模式
UART1TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;//外设： 8 位
UART1TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE; //存储器： 8 位
UART1TxDMA_Handler.Init.Mode=DMA_NORMAL; //普通模式
UART1TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM; //中等优先级
UART1TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;
UART1TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
UART1TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE; //存储器突发单次传输
UART1TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE; //外设突发单次传输
```

这里大家要注意， HAL 库为了处理各类外设的 DMA 请求，在调用相关函数之前，需要调用一个宏定义标识符，来连接 DMA 和外设句柄。例如要使用串口 DMA 发送，所以方式为：

```c
__HAL_LINKDMA(&UART1_Handler,hdmatx,UART1TxDMA_Handler);
```

其中 UART1_Handler 是串口初始化句柄， 我们在 usart.c 中定义过了。UART1TxDMA_Handler是 DMA 初始化句柄。 hdmatx 是外设句柄结构体的成员变量， 在这里实际就是 UART1_Handler 的成员变量。 在 HAL 库中，任何一个可以使用 DMA 的外设，它的初始化结构体句柄都会有一个DMA_HandleTypeDef 指针类型的成员变量，是 HAL 库用来做相关指向的。 Hdmatx 就是DMA_HandleTypeDef 结构体指针类型。

这句话的含义就是把 UART1_Handler 句柄的成员变量 hdmatx 和 DMA 句柄 UART1TxDMA_Handler 连接起来，是纯软件处理，没有任何硬件操作。

### 3.3 使能串口1的DMA发送

串口 1 的 DMA 发送实际是串口控制寄存器 CR3 的位 7 来控制的，在 HAL 库中，多次操作该寄存器来使能串口 DMA 发送，但是它并没有提供一个独立的使能函数，所以这里我们可以通过直接操作寄存器方式来实现：

```c
USART1->CR3 |= USART_CR3_DMAT;//使能串口 1 的 DMA 发送
```

HAL 库还提供了对串口的 DMA 发送的停止，暂停，继续等操作函数：

```c
HAL_StatusTypeDef HAL_UART_DMAStop(UART_HandleTypeDef *huart); //停止
HAL_StatusTypeDef HAL_UART_DMAPause(UART_HandleTypeDef *huart); //暂停
HAL_StatusTypeDef HAL_UART_DMAResume(UART_HandleTypeDef *huart);//恢复
```

### 3.4 使能DMA2数据流7，启动传输

使能 DMA 数据流的函数为：

```c
HAL_StatusTypeDef HAL_DMA_Start(DMA_HandleTypeDef *hdma, uint32_t SrcAddress,
                                uint32_t DstAddress, uint32_t DataLength);
```

这个函数比较好理解，第一个参数是 DMA 句柄，第二个是传输源地址，第三个是传输目标地址，第四个是传输的数据长度。

通过以上 4 步设置，我们就可以启动一次 USART1 的 DMA 传输了。

### 3.5 查询DMA传输状态

在 DMA 传输过程中，我们要查询 DMA 传输通道的状态，使用的方法是：

```c
__HAL_DMA_GET_FLAG(&UART1TxDMA_Handler,DMA_FLAG_TCIF3_7);
```

获取当前传输剩余数据量：

```c
__HAL_DMA_GET_COUNTER(&UART1TxDMA_Handler);
```

同样，我们也可以设置对应的 DMA 数据流传输的数据量大小,函数为：

```c
__HAL_DMA_SET_COUNTER(&UART1TxDMA_Handler,1000);
```

### 3.6 DMA中断使用方法

DMA 中断对于每个流都有一个中断服务函数，比如 DMA2_Stream7 的中断服务函数为DMA2_Stream7_IRQHandler。同样，HAL库也提供了一个通用的DMA中断处理函数HAL_DMA_IRQHandler，在该函数内部，会对 DMA 传输状态进行分析，然后调用相应的中断处理回调函数：

```c
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);    // 发送完成回调函数
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart);// 发送一半回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);    // 接收完成回调函数
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);// 接收一半回调函数
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);     // 传输出错回调函数
```

对于串口 DMA 开启，使能数据流，启动传输，这些步骤，如果使用了中断，可以直接调用 HAL 库函数 HAL_USART_Transmit_DMA，该函数声明如下：

```c
HAL_StatusTypeDef HAL_USART_Transmit_DMA(USART_HandleTypeDef *husart,
                                         uint8_t *pTxData, uint16_t Size);
```
