# 第十七章 DMA外设到存储器实验

## 1. 硬件设计

外设到存储器使用到USART1功能，具体电路设置参考USART章节，无需其他硬件设计。

## 2. 软件设计

### 2.1 编程大纲

- 配置USART通信功能；

- 设置串口DMA工作参数；

- 使能DMA；

- DMA传输同时CPU可以运行其他任务。

### 2.2 代码分析

#### 2.2.1 DMA和串口宏定义

```c
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

// 中断相关宏定义
#define USARTx_IRQn USART1_IRQn
#define USARTx_IRQHandler USART1_IRQHandler

// DMA相关参数宏定义
#define USART_TX_DMA_CHANNEL DMA1_Channel5 
#define USART_DR_ADDRESS (USART1_BASE+0x04) // USART1数据寄存器地址
#define ReceBuff_Size 5000 // 发送缓冲区大小
```

#### 2.2.2 串口配置

```c
// USART配置函数
void USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    // 时钟使能
    USARTx_GPIO_APBxClkCmd(USARTx_GPIO_CLK, ENABLE);
    USARTx_APBxCLKCmd(USARTx_CLK, ENABLE);
    // NVIC 配置
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置中断优先级组2
    NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn; // 选择USARTx的中断请求
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能USARTx的中断请求
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
    NVIC_Init(&NVIC_InitStructure);
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
    // 使能空闲中断
    USART_ITConfig(USARTx, USART_IT_IDLE, ENABLE);
    USART_Cmd(USARTx, ENABLE); // 使能串口
}
```

相比于上一节，因为我们需要使用到中断，NVIC需要重新配置，注意一个重要库函数：

```c
USART_ITConfig(USARTx, USART_IT_IDLE, ENABLE);
```

##### 2.2.2.1 函数名：`USART_ITConfig`

- **`USART_ITConfig`** 是一个库函数，用于配置指定 USART 外设的中断。此函数的作用是使能或禁用指定的中断事件。

##### 2.2.2.2 参数：

1. **`USARTx`**：指定目标 USART 外设。`x` 可以是数字，代表不同的 USART 外设，比如 `USART1`、`USART2`、`USART3` 等。
   
   示例：
   
   - `USART1`
   - `USART2`
   - `USART3`
   
   你可以根据你的硬件配置选择要操作的 USART 外设。

2. **`USART_IT_IDLE`**：指定要配置的中断类型。`USART_IT_IDLE` 表示配置空闲中断。USART 空闲中断在接收到数据流停止时触发，常用于判断串口传输的结束，或者用于检测数据流的空闲状态。

3. **`ENABLE`**：指示是否启用该中断。`ENABLE` 启用中断，`DISABLE` 禁用中断。

##### 2.2.2.3 空闲中断（Idle Interrupt）

- **空闲中断**是 USART 外设的一个特殊中断，当 USART 外设检测到数据流停止（即接收线路处于空闲状态）时，它会触发该中断。通常，当串口的接收通道闲置（没有接收到数据）时，这个中断会被触发。

- 空闲中断的应用场景：这种中断可以用来检测数据接收过程的结束，或者实现一些特殊的数据处理策略，比如根据特定的时间间隔来判断数据接收是否已完成。

#### 2.2.3 DMA配置

```c
void USARTx_DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;    
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ReceiveBuff;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;    
    DMA_InitStructure.DMA_BufferSize = ReceBuff_Size;  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;     
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;           
    DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);   
    DMA_Cmd(USART_TX_DMA_CHANNEL,ENABLE);
}
```

#### 2.2.4 中断服务函数

```c
void USART1_IRQHandler(void)
{
    uint16_t t;
    if(USART_GetITStatus(USARTx, USART_IT_IDLE) == SET)         
    {    
        DMA_Cmd(USART_TX_DMA_CHANNEL,DISABLE);                         
        t = DMA_GetCurrDataCounter(USART_TX_DMA_CHANNEL);              
        USART_SendArray(USARTx, ReceiveBuff, ReceBuff_Size-t);       
        DMA_SetCurrDataCounter(USART_TX_DMA_CHANNEL,ReceBuff_Size);    
        DMA_Cmd(USART_TX_DMA_CHANNEL,ENABLE);                          
        USART_ReceiveData(USARTx);                              
        USART_ClearFlag(USARTx,USART_FLAG_IDLE);        
    }    
}
```

工作流程：

1. 当 USART 发生空闲中断时，DMA 数据传输被禁用。
2. 根据 DMA 剩余的传输字节数（`t`），将接收到的数据发送出去。
3. 重新配置 DMA，使其继续接收数据。
4. 清除 USART 的 IDLE 标志，为下次中断做好准备。

#### 2.2.5 主函数测试

```c
#include "led.h"
#include "dma.h"
#include "usart.h"

static void Delay(__IO uint32_t nCount)     //简单的延时函数
{
    for(; nCount != 0; nCount--);
}

int main(void)
{
    LED_Init();
    USARTx_DMA_Config();
    USART_Config();
    USART_SendString(USARTx, "\r\nDMA USART1 Test!\r\n");
    USART_DMACmd(USARTx, USART_DMAReq_Rx, ENABLE);
    while(1)
    {
        LED1_ON();
        Delay(0xFFFFFF);
        LED1_OFF();
        Delay(0xFFFFFF);
    }
}
```

## 3. 小结

这章功能就好像我们已经讲过的串口接发实验一样，但是使用DMA不会占用cpu资源，适合大型项目，更多内容参考注释。DMA实验也已经写了三章，再来复习一下配置吧，以这个实验为例：

DMA 从外设（如串口）到存储器的配置允许外设数据自动传输到内存中，无需 CPU 干预。以下是配置步骤和示例

### 3.1 配置步骤

1. **开启 DMA 时钟**：

```c
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
```

2. **配置 DMA 通道**：
   设置 DMA 外设地址（如串口的数据寄存器地址）、内存地址、数据方向等。

```c
DMA_InitTypeDef DMA_InitStructure;
DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART1->DR)); // 外设地址
DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ReceiveBuff; // 内存地址
DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; // 外设到内存
DMA_InitStructure.DMA_BufferSize = RECEIVEBUFF_SIZE; // 数据缓冲区大小
DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不增
DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址增
DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据大小
DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 内存数据大小
DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 循环模式
DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // 优先级
DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 仅外设到内存
```

3. **初始化 DMA**：

```c
DMA_Init(DMA1_Channel5, &DMA_InitStructure); // 初始化特定 DMA 通道
```

4. **使能 DMA**：

```c
DMA_Cmd(DMA1_Channel5, ENABLE);
```

5. **使能 USART DMA 接收**：

```c
USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
```

### 3.2 示例代码

假设你要将 USART1 的接收到的数据存储到 `ReceiveBuff` 缓冲区中：

```c
#include "stm32f10x.h"

#define RECEIVEBUFF_SIZE  256
uint8_t ReceiveBuff[RECEIVEBUFF_SIZE];

void DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    // 使能 DMA 时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // DMA 配置
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART1->DR)); // 外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ReceiveBuff; // 内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; // 从外设到内存
    DMA_InitStructure.DMA_BufferSize = RECEIVEBUFF_SIZE; // 缓冲区大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据大小
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 内存数据大小
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // 优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 仅外设到内存
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    // 使能 DMA
    DMA_Cmd(DMA1_Channel5, ENABLE);

    // 使能 USART DMA 接收
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
}

int main(void)
{
    // USART 和 DMA 配置
    USART_InitTypeDef USART_InitStructure;
    // 配置 USART1 (省略具体配置)
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);

    DMA_Config();

    while (1)
    {
        // 主循环
        // 你可以在这里处理接收到的数据
    }
}
```

---

2024.8.29 第一次修订，后期不再维护

2025.1.14 新增了许多内容，代码优化
