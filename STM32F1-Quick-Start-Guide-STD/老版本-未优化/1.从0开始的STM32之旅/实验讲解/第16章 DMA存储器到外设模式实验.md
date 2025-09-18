# 第十六章 DMA存储器到外设模式实验

## 1. 硬件设计

存储器到外设模式使用到USART1功能，具体电路设置参考USART章节，无需其他硬件设计。

## 2. 软件设计

### 2.1 编程大纲

1. 配置USART通信功能

2. 设置串口DMA工作参数

3. 使能DMA

4. DMA传输同时CPU可以运行其他任务

### 2.2 代码分析

#### 2.2.1 串口和DMA的宏定义

```c
#ifndef __DMA_H
#define __DMA_H

#include "led.h"

// DMA相关参数宏定义
#define USART_TX_DMA_CHANNEL DMA1_Channel4 // 使用DMA1通过通道4传输USART1发送数据
#define USART_DR_ADDRESS (USART1_BASE+0x04) // USART1数据寄存器地址
#define SendBuff_Size 5000 // 发送缓冲区大小

void USARTx_DMA_Init(void); // USART1 DMA初始化函数

#endif /* __DMA_H */
```

```c
#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h"

// USARTx 引脚宏定义
#define USARTx_GPIO_CLK RCC_APB2Periph_GPIOA
#define USARTx_GPIO_APBxClkCmd RCC_APB2PeriphClockCmd
// TX-发送数据输出引脚
#define USARTx_TX_GPIO_PORT GPIOA
#define USARTx_TX_GPIO_PIN GPIO_Pin_9
// RX-接收数据输入引脚
#define USARTx_RX_GPIO_PORT GPIOA
#define USARTx_RX_GPIO_PIN GPIO_Pin_10

// USARTx 相关参数宏定义
#define USARTx  USART1
#define USARTx_CLK RCC_APB2Periph_USART1
#define USARTx_APBxCLKCmd RCC_APB2PeriphClockCmd
#define USARTx_BaudRate 115200

// 一些函数声明
void USART_Config(void);
void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch);
void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *pArray, uint16_t len);
void USART_SendString(USART_TypeDef *pUSARTx, uint8_t *pStr);
void USART_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch);

#endif
```

使用宏定义设置外设配置方便程序修改和升级。

USART部分设置与USART章节内容相同，可以参考USART章节内容理解。串口的发送请求对应有固定的DMA通道， 这里的外设的地址即串口的数据寄存器，一次要发送的数据量可以自定义

#### 2.2.2 串口初始化

```c
// USART配置函数
void USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    USARTx_GPIO_APBxClkCmd(USARTx_GPIO_CLK, ENABLE);
    USARTx_APBxCLKCmd(USARTx_CLK, ENABLE);
    // TX引脚配置
    GPIO_InitStructure.GPIO_Pin = USARTx_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);
    // RX引脚配置
    GPIO_InitStructure.GPIO_Pin = USARTx_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 输入浮空
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);
    // USART 配置
    USART_InitStructure.USART_BaudRate = USARTx_BaudRate; // 波特率115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 字长8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
    USART_Init(USARTx, &USART_InitStructure);
    USART_Cmd(USARTx, ENABLE); // 使能串口
}
```

和前面章节大差不差，我们删除了NVIC相关和中断，这里主要是使用DMA传输。

#### 2.2.3 USARTx_DMA配置

```c
#include "dma.h"

uint8_t SendBuff[SendBuff_Size]; // 发送缓冲区

// 本工程是内存到外设（USART1->DR)
void USARTx_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // 设置DMA源数据地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
    // 设置内存地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SendBuff;
    // 设置方向
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    // 设置传输长度
    DMA_InitStructure.DMA_BufferSize = SendBuff_Size;
    //外设地址不增，内存地址递增
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // 设置外设数据单位和内存数据单位
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    // 一次模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    // 禁止内存到内存的传输
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);
    DMA_Cmd(USART_TX_DMA_CHANNEL, ENABLE);
}
```

#### 2.2.4 主函数测试

```c
#include "led.h"
#include "usart.h"
#include "dma.h"

extern uint8_t SendBuff[SendBuff_Size];
static void Delay(__IO uint32_t nCount) // 简单的延时函数
{
    for(; nCount != 0; nCount--);
}

int main()
{
    uint16_t i;
    LED_Init();
    USART_Config();
    USARTx_DMA_Init();
    // for循环填充要发送的数据
    for(i = 0; i < SendBuff_Size; i++)
    {
        SendBuff[i] = 'A';
    }
    // USART向DMA发出TX请求
    USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
    while(1)
    {
        // DMA不消耗CPU资源，我们可以做其他事，这里我们只是让LED闪烁
        LED1_ON();
        Delay(0xFFFFFF);
        LED1_OFF();
        Delay(0xFFFFFF);
    }
}
```

USART_DMACmd函数用于控制USART的DMA请求的启动和关闭。它接收三个参数，第一个参数用于设置串口外设，可以是USART1/2/3和UART4/5这5个参数可选， 第二个参数设置串口的具体DMA请求，有串口发送请求USART_DMAReq_Tx和接收请求USART_DMAReq_Rx可选， 第三个参数用于设置启动请求ENABLE或者关闭请求DISABLE。运行该函数后USART的DMA发送传输就开始了，根据配置存储器的数据会发送到串口。

DMA传输过程是不占用CPU资源的，可以一边传输一次运行其他任务。

## 3. 小结

有了上一章的基础，这一次应该更容易理解，对照注释清晰明了，不同的是我们这是内部存储到外设，改一下配置就行啦，其他无异

主函数里有一个库函数我们需要详细说明一下：

### 3.1 函数原型

```c
void USART_DMACmd(USART_TypeDef* USARTx, FunctionalState NewState);
```

### 3.2 参数解释

- **`USARTx`**: 这是 USART 外设的指针。例如，`USART1`、`USART2` 等。
- **`USART_DMAReq`**: 这是 DMA 请求的类型。通常有两种类型：`USART_DMAReq_Tx`（发送请求）和 `USART_DMAReq_Rx`（接收请求）。
- **`NewState`**: 指定要启用还是禁用 DMA 请求。可以是 `ENABLE`（启用）或 `DISABLE`（禁用）。

### 3.3 工作原理

1. **启用 DMA 请求**: 当启用 DMA 请求时，USART 在发送或接收数据时会触发 DMA 控制器，从而自动处理数据的传输，而不是由 CPU 直接进行传输。这使得数据传输更高效，特别是在大量数据传输时。

2. **禁用 DMA 请求**: 禁用 DMA 请求后，USART 将不再自动使用 DMA 进行数据传输，数据将由 CPU 直接处理。

### 3.4 示例代码

下面是一个简单的例子，展示如何启用 USART1 的 DMA 发送请求：

```c
// 使能 USART1 的 DMA 发送请求
USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

// 假设 USART1 已经被初始化并且 DMA 也配置好了
// 现在可以将数据通过 DMA 发送

// 发送数据
const uint8_t data[] = "Hello, DMA!";
DMA_ConfigureForUSART1Tx(data, sizeof(data)); // 这是一个假设的函数，用于配置 DMA 发送数据

// 启动 DMA 传输
DMA_Cmd(DMA1_Channel4, ENABLE); // 使能 DMA 通道，假设使用的是 DMA1 通道4
```

### 3.5 解释

1. **`USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);`**: 启用 USART1 的 DMA 发送请求。
2. **`DMA_ConfigureForUSART1Tx(data, sizeof(data));`**: 配置 DMA 以便它可以发送 `data` 数组中的数据。实际代码中，需要设置 DMA 的源地址、目的地址、传输长度等参数。
3. **`DMA_Cmd(DMA1_Channel4, ENABLE);`**: 启动 DMA 通道以开始数据传输。

---

2024.8.29 第一次修订，后期不在维护

2025.1.14 内容简化，程序修改
