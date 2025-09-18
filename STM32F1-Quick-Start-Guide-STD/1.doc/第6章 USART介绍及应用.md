# 第六章 USART介绍及应用

## 1. USART简介

通用同步异步收发器 (Universal Synchronous Asynchronous Receiver and Transmitter) 是一个串行通信设备，可以灵活地与外部设备进行全双工数据交换。有别于 USART 还有一个 UART(Universal Asynchronous Receiver and Transmitter)，它是在 USART 基础上裁剪掉了同步通信功能，只有异步通信。简单区分同步和异步就是看通信时需不需要对外提供时钟输出，我们平时用的串口通信基本都是 UART。

串行通信一般是以帧格式传输数据，即是一帧一帧的传输，每帧包含有起始信号、数据信息、停止信息，可能还有校验信息。 USART 就是对这些传输参数有具体规定，当然也不是只有唯一一个参数值，很多参数值都可以自定义设置，只是增强它的兼容性。

USART 满足外部设备对工业标准 NRZ 异步串行数据格式的要求，并且使用了小数波特率发生器，可以提供多种波特率，使得它的应用更加广泛。 USART 支持同步单向通信和半双工单线通信；还支持局域互连网络 LIN、智能卡 (SmartCard) 协议与 lrDA(红外线数据协会) SIR ENDEC 规范。

USART 支持使用 DMA，可实现高速数据通信，有关 DMA 具体应用将在 DMA 章节作具体讲解。USART 在 STM32 应用最多莫过于“打印”程序信息，一般在硬件设计时都会预留一个 USART

通信接口连接电脑，用于在调试程序是可以把一些调试信息“打印”在电脑端的串口调试助手工具上，从而了解程序运行是否正确、如果出错哪具体哪里出错等等。

## 2. USART应用示例

USART 只需两根信号线即可完成双向通信，对硬件要求低，使得很多模块都预留 USART 接口来实现与其他模块或者控制器进行数据传输，比如 GSM 模块， WIFI 模块、蓝牙模块等等。在硬件设计时，注意还需要一根“共地线”。

我们经常使用 USART 来实现控制器与电脑之间的数据传输。这使得我们调试程序非常方便，比如我们可以把一些变量的值、函数的返回值、寄存器标志位等等通过 USART 发送到串口调试助手，这样我们可以非常清楚程序的运行状态，当我们正式发布程序时再把这些调试信息去除即可

我们不仅仅可以将数据发送到串口调试助手，我们还可以在串口调试助手发送数据给控制器，控制器程序根据接收到的数据进行下一步工作。

首先，我们来编写一个程序实现开发板与电脑通信，在开发板上电时通过 USART 发送一串字符串给电脑，然后开发板进入中断接收等待状态，如果电脑有发送数据过来，开发板就会产生中断，我们在中断服务函数接收数据，并马上把数据返回发送给电脑。

为利用 USART 实现开发板与电脑通信，需要用到一个 USB 转 USART 的 IC，我们选择 CH340G芯片来实现这个功能， CH340G 是一个 USB 总线的转接芯片，实现 USB 转 USART、 USB 转 lrDA红外或者 USB 转打印机接口，我们使用其 USB 转 USART 功能。

我们将 CH340G 的 TXD 引脚与 USART1 的 RX 引脚连接， CH340G 的 RXD 引脚与 USART1 的TX 引脚连接。 CH340G 芯片集成在开发板上，其地线 (GND) 已与控制器的 GND 连通。

![屏幕截图 2025-06-01 114459.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/06/01-11-45-10-屏幕截图%202025-06-01%20114459.png)

### 2.1 数据接发中断回显

#### 2.1.1 usart相关参数宏定义

```c
#ifndef __USART_H__
#define __USART_H__

#include "stm32f10x.h"

#define USARTx USART1
#define USARTx_CLK RCC_APB2Periph_USART1
#define USARTx_GPIO_CLK RCC_APB2Periph_GPIOA
#define USARTx_GPIO_Port GPIOA
#define USARTx_Tx_Pin GPIO_Pin_9
#define USARTx_Rx_Pin GPIO_Pin_10

#define USARTx_IRQn USART1_IRQn
#define USARTx_IRQHandler USART1_IRQHandler

void USARTx_Init(void);
void usart_sendbyte(USART_TypeDef* pUSARTx, uint8_t ch);
void usart_sendstring(USART_TypeDef* pUSARTx, const char *str);

#endif /*  __USART_H__ */
```

#### 2.1.2 usart工作参数及中断配置

```c
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void USARTx_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // 开启USART1和GPIOA的时钟
    RCC_APB2PeriphClockCmd(USARTx_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(USARTx_CLK, ENABLE);

    // 配置USART1 TX（输出）
    GPIO_InitStructure.GPIO_Pin = USARTx_Tx_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USARTx_GPIO_Port, &GPIO_InitStructure);

    // 配置USART1 RX（输入）
    GPIO_InitStructure.GPIO_Pin = USARTx_Rx_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USARTx_GPIO_Port, &GPIO_InitStructure);

    // 配置USART
    USART_InitStructure.USART_BaudRate = 115200; // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 字节长度
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 停止位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 接收发送使能
    USART_Init(USARTx, &USART_InitStructure);

    // 配置中断
    NVIC_Configuration();
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE); // 使能接收中断
    USART_Cmd(USARTx, ENABLE);
}
```

#### 2.1.3 usart发送数据函数

```c
void usart_sendbyte(USART_TypeDef* pUSARTx, uint8_t ch)
{
    USART_SendData(pUSARTx, ch); // 发送数据
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET); // 等待发送完毕
}

void usart_sendstring(USART_TypeDef* pUSARTx, const char *str)
{
    while(*str)  // 逐个字符发送
    {
        usart_sendbyte(pUSARTx, *str);
        str++;
    }
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET); // 等待发送完成
}
```

#### 2.1.4 中断服务函数

```c
void USARTx_IRQHandler(void)
{
    uint8_t temp;
    if(USART_GetITStatus(USARTx, USART_IT_RXNE) != RESET) // 如果接收到数据
    {
        temp = USART_ReceiveData(USARTx); // 读取接收到的数据
        usart_sendbyte(USARTx, temp); // 回显接收到的数据
    }
}
```

#### 2.1.5 主函数测试

```c
#include "stm32f10x.h"
#include "usart.h"
#include "SysTick.h"

int main()
{
    SysTick_Init();
    USARTx_Init();
    usart_sendstring(USARTx, "hello world\r\n");
    while(1)
    {
        usart_sendstring(USARTx, "hello world\r\n");
        Delay_ms(1000);
    }
}
```

### 2.2 指令控制LED

#### 2.2.1 重定向printf和scanf函数

```c
int fputc(int ch, FILE *f)
{
    usart_sendbyte(USARTx, (uint8_t)ch);
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    return (ch);
}

int fgetc(FILE *f)
{
    while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
    return (int)USART_ReceiveData(USARTx);
}
```

#### 2.2.2 打印指令菜单

```c
static void show_info(void)
{
  printf("\r\n LED Control Commands :\r\n");
  printf("1. RED LED ON\r\n");
  printf("2. GREEN LED ON\r\n");
  printf("3. BLUE LED ON\r\n");
  printf("4. RED LED OFF\r\n");
  printf("5. GREEN LED OFF\r\n");
  printf("6. BLUE LED OFF\r\n");
  printf("7. TURN OFF ALL LEDS");
}
```

#### 2.2.3 主函数测试

```c
int main()
{
  char ch;
    SysTick_Init();
  BSP_LED_Init();
  USARTx_Init();
  show_info();
  while(1)
  {
    ch = getchar();
        switch(ch)
    {
      case '1':
        LED_ON(RED_LED_Pin);
        break;
      case '2':
        LED_ON(GREEN_LED_Pin);
        break;
      case '3':
        LED_ON(BLUE_LED_Pin);
        break;
      case '4':
        LED_OFF(RED_LED_Pin);
        break;
      case '5':
        LED_OFF(GREEN_LED_Pin);
        break;
      case '6':
        LED_OFF(BLUE_LED_Pin);
        break;
      case '7':
        LED_OFF(RED_LED_Pin);
        LED_OFF(GREEN_LED_Pin);
        LED_OFF(BLUE_LED_Pin);
        break;
      default:
        break;  
    }
  }
}
```

## 3. USART常见函数（STD库）

### **3.1 USART初始化与配置**

#### **(1) 使能时钟**

```c
// USART1在APB2总线，USART2/3在APB1总线
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  // 使能USART1时钟
RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  // 使能USART2时钟
```

#### **(2) 配置GPIO引脚**

```c
// 以USART1为例（PA9-TX, PA10-RX）
GPIO_InitTypeDef GPIO_InitStruct;
// TX引脚配置（复用推挽输出）
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &GPIO_InitStruct);

// RX引脚配置（浮空输入）
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入
GPIO_Init(GPIOA, &GPIO_InitStruct);
```

#### **(3) USART参数配置**

```c
USART_InitTypeDef USART_InitStruct;
USART_InitStruct.USART_BaudRate = 115200;              // 波特率
USART_InitStruct.USART_WordLength = USART_WordLength_8b; // 8位数据
USART_InitStruct.USART_StopBits = USART_StopBits_1;    // 1位停止位
USART_InitStruct.USART_Parity = USART_Parity_No;       // 无校验
USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 使能收发
USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无流控

USART_Init(USART1, &USART_InitStruct);  // 应用配置
```

#### **(4) 使能USART**

```c
USART_Cmd(USART1, ENABLE);  // 使能USART1
```

### **3.2 数据收发函数**

#### **(1) 发送数据**

```c
// 发送单个字节（阻塞式）
void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
```

#### **(2) 接收数据**

```c
// 接收单个字节
uint16_t USART_ReceiveData(USART_TypeDef* USARTx);
```

#### **(3) 发送字符串**

```c
void USART_SendString(USART_TypeDef* USARTx, char *str) {
    while (*str) {
        USART_SendData(USARTx, *str++);
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    }
}
```

### **3.3 中断配置**

#### **(1) 使能中断**

```c
// 使能接收中断
USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

// 使能发送完成中断（可选）
USART_ITConfig(USART1, USART_IT_TC, ENABLE);
```

#### **(2) 配置NVIC**

```c
NVIC_InitTypeDef NVIC_InitStruct;
NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStruct);
```

#### **(3) 中断服务函数**

```c
void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        // 接收中断处理
        uint8_t data = USART_ReceiveData(USART1);
        // 处理接收到的数据...
    }

    if (USART_GetITStatus(USART1, USART_IT_TC) != RESET) {
        // 发送完成中断处理
        USART_ClearITPendingBit(USART1, USART_IT_TC);
    }
}
```

### **4. 状态标志操作**

#### **(1) 获取状态标志**

```c
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);
```

常用标志：

- `USART_FLAG_TXE`：发送数据寄存器空

- `USART_FLAG_TC`：发送完成

- `USART_FLAG_RXNE`：接收数据寄存器非空

- `USART_FLAG_ORE`：溢出错误

#### **(2) 清除标志**

```c
void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);
```

#### **(3) 中断状态检查**

```c
ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
```

### **5. 高级功能**

#### **(1) DMA配置**

```c
// 使能USART DMA发送
USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

// 使能USART DMA接收
USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
```

#### **(2) 硬件流控**

```c
// 启用RTS/CTS流控
USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
```

#### **(3) 多处理器通信**

```c
// 设置地址
USART_SetAddress(USART1, 0x02);

// 启用地址检测
USART_WakeUpConfig(USART1, USART_WakeUp_AddressMark);
```
