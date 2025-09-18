# 第十二章 USART1接发通信实验

## 1. 硬件设计

为利用USART实现开发板与电脑通信，需要用到一个USB转USART的IC，我们选择CH340G芯片来实现这个功能，CH340G是一个USB总线的转接芯片， 实现USB转USART、USB转lrDA红外或者USB转打印机接口，我们使用其USB转USART功能。

我们将CH340G的TXD引脚与USART1的RX引脚连接，CH340G的RXD引脚与USART1的TX引脚连接。CH340G芯片集成在开发板上，其地线(GND)已与控制器的GND连通。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART011.png)

## 2. 软件设计

### 2.1 编程大纲

1. 使能RX和TX引脚GPIO时钟和USART时钟；

2. 初始化GPIO，并将GPIO复用到USART上；

3. 配置USART参数；

4. 配置中断控制器并使能USART接收中断；

5. 使能USART；

6. 在USART接收中断服务函数实现数据接收和发送。

### 2.2 代码分析

#### 2.2.1 USART1头文件配置-设置端口、引脚、中断

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

// NVIC配置
#define USARTx_IRQ  USART1_IRQn
#define USARTx_IRQHandler USART1_IRQHandler
```

使用宏定义方便程序移植和升级 。开发板中的CH340G的收发引脚默认通过跳帽连接到USART1，如果想使用其他串口， 可以把CH340G跟USART1直接的连接跳帽拔掉，然后再把其他串口的IO用杜邦线接到CH340G的收发引脚即可。

这里我们使用USART1，设定波特率为115200，选定USART的GPIO为PA9和PA10。

#### 2.2.2 配置NVIC

```c
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
```

在中断章节已对嵌套向量中断控制器的工作机制做了详细的讲解，这里我们就直接使用，配置USART作为中断源，因为本实验没有使用其他中断，对优先级没什么具体要求。

#### 2.2.3 串口中断服务函数

```c
// 串口中断服务函数
void USART1_IRQHandler(void)
{
  uint8_t TempData;
  if(USART_GetITStatus(USARTx, USART_IT_RXNE)!= RESET)
  {
    TempData = USART_ReceiveData(USARTx);
    USART_SendData(USARTx, TempData);
  }
}
```

#### 2.2.4 配置USART GPIO及工作参数设置

```c
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
    GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);
    // USART 配置
    USART_InitStructure.USART_BaudRate = USARTx_BaudRate; // 波特率115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 字长8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
    USART_Init(USARTx, &USART_InitStructure);
    // NVIC配置
    NVIC_Configuration();
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE); // 使能串口接收中断
    USART_Cmd(USARTx, ENABLE); // 使能串口
}
```

使用GPIO_InitTypeDef和USART_InitTypeDef结构体定义一个GPIO初始化变量以及一个USART初始化变量，这两个结构体内容我们之前已经有详细讲解。

调用RCC_APB2PeriphClockCmd函数开启GPIO端口时钟，使用GPIO之前必须开启对应端口的时钟。使用RCC_APB2PeriphClockCmd函数开启USART时钟。

使用GPIO之前都需要初始化配置它，并且还要添加特殊设置，因为我们使用它作为外设的引脚，一般都有特殊功能。 我们在初始化时需要把它的模式设置为复用功能。这里把串口的Tx引脚配置为复用推挽输出，Rx引脚为浮空输入，数据完全由外部输入决定。

接下来，我们配置USART1通信参数为：波特率115200，字长为8，1个停止位，没有校验位，不使用硬件流控制，收发一体工作模式，然后调用USART初始化函数完成配置。

程序用到USART接收中断，需要配置NVIC，这里调用NVIC_Configuration函数完成配置。配置完NVIC之后调用USART_ITConfig函数使能USART接收中断。

最后调用USART_Cmd函数使能USART，这个函数最终配置的是USART_CR1的UE位，具体的作用是开启USART的工作时钟，没有时钟那USART这个外设自然就工作不了。

照例，我们应该分析一下新出现的库函数：

1. **`NVIC_Configuration();`**：
   
   - 这个函数通常用于配置嵌套向量中断控制器（NVIC）的中断优先级。通过设置中断优先级和中断使能状态，它控制哪个中断源的优先级更高，以及在中断请求时如何响应。

2. **`USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);`**：
   
   - `USART_ITConfig` 函数用于启用或禁用特定 USART 的中断。在这里，`USART_IT_RXNE` 表示接收数据寄存器非空中断，使能这个中断后，每当接收到新数据时，会触发中断请求。`ENABLE` 表示启用这个中断。

3. **`USART_Cmd(DEBUG_USARTx, ENABLE);`**：
   
   - `USART_Cmd` 函数用于使能或禁用指定的 USART 外设。`ENABLE` 参数表示启用 USART，使其开始工作，接收和发送数据。

#### 2.2.5 编写一些数据发送函数

```c
// 发送一个字节函数
// 函数参数：USARTx：串口号，ch：发送的字节
void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
{
    USART_SendData(pUSARTx, ch);
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET); // 等待发送完成
}

// 发送一个8位数组函数
// 函数参数：USARTx：串口号，data：发送的8位数据
void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *pArray, uint16_t len)
{
    uint8_t i;
    for(i = 0; i < len; i++)
    {
        USART_SendByte(pUSARTx, pArray[i]); // 循环写入数组
    }
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET); // 等待发送完成
}

// 发送一个字符串函数
// 函数参数：USARTx：串口号，pStr：发送的字符串
void USART_SendString(USART_TypeDef *pUSARTx, uint8_t *pStr)
{
    unsigned int k = 0;
    do{
        USART_SendByte(pUSARTx, *(pStr+k)); // 循环写入字符串
        k++;
    }while(*(pStr+k)!= '\0'); // 直到遇到结束符‘\0’
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

// 发送一个16位数据函数
// 函数参数：USARTx：串口号，ch：发送的16位数据
void USART_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch)
{
    uint8_t temp_h, temp_l; // 16位数据要分为高位和低位，两次发送
    temp_h = (ch & 0xff00) >> 8; // 取高位
    temp_l = ch & 0x00ff; // 取低位
    USART_SendByte(pUSARTx, temp_h); // 发送高位
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
    USART_SendByte(pUSARTx, temp_l); // 发送低位
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}
```

#### 2.2.6 重定向printf和scanf函数到串口-目的是让串口也可以使用

```c
// 重定向printf和scanf函数到串口
int USART_PutChar(int ch, FILE *f) // 函数参数：ch：要发送的字符，f：文件指针
{
    USART_SendByte(USARTx, (uint8_t)ch); // 发送字符
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    return ch; // 返回发送的字符
}

int USART_GetChar(FILE *f) // 函数参数：f：文件指针
{
    while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
    return (int)USART_ReceiveData(USARTx); // 接收字符
}
```

#### 2.2.7 主函数测试

```c
#include "usart.h"
#include "stm32f10x.h"

int main(void)
{
  USART_Config();
  // 测试发送一个字符串
  USART_SendString(USARTx, "这是一个串口测试程序\r\n");
  while(1)
  {

  }
}
```

## 3. 小结

注释已经说得很多了，我们还是总结一下基本流程吧：

- 首先就算USART1的头文件了，我们需要在这里宏定义各种串口需要的资源（GPIO、RCC、NVIC）

- 接着写我们的串口函数了，首先就算初始化NVIV和GPIO了，接着再写我们的功能函数

- 主函数就简单了，直接引用我们已经写好的函数就ok咯

---

2024.8.24 第一次修订，后期不再维护

2024.12.22 整体大改，把第一版代码很多不必要的删了
