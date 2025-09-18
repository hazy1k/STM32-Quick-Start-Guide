# 第十三章 USART1指令控制RGB彩灯实验

## 1. 硬件设计

同前一章实验和已经介绍过的led

## 2. 软件设计

### 2.1 编程大纲

1. 初始化配置RGB彩色灯GPIO；

2. 使能RX和TX引脚GPIO时钟和USART时钟；

3. 初始化GPIO，并将GPIO复用到USART上；

4. 配置USART参数，使能USART；

5. 获取指令输入，根据指令控制RGB彩色灯。

### 2.2 代码分析

USART1和led控制我们已经详细的分析过了，下面回顾一下重要部分，讲解主函数

#### 2.2.1 串口配置

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

#### 2.2.2 主函数

```c
#include "led.h"
#include "usart.h"

static void Show_Info(void) // 显示RGB彩灯控制信息
{
    USART_SendString(USARTx, "Please input the RGB value:\r\n");
    USART_SendString(USARTx, "1  -----   红色\n");
    USART_SendString(USARTx, "2  -----   绿色\n");
    USART_SendString(USARTx, "3  -----   蓝色\n");
    USART_SendString(USARTx, "4  -----   黄色\n");
    USART_SendString(USARTx, "5  -----   紫色\n");
    USART_SendString(USARTx, "6  -----   青色\n");
    USART_SendString(USARTx, "7  -----   白色\n");
    USART_SendString(USARTx, "8  -----   关闭\n");
}

int main(void)
{
    char temp;
    // 初始化外设，打印提示信息
    LED_Init();
    USART_Config();
    Show_Info();
    while(1)
    {
        temp = USART_ReceiveData(USARTx);
        switch(temp)
        {
            case '1': LED_RED(); break;
            case '2': LED_GREEN(); break;
            case '3': LED_BLUE(); break;
            case '4': LED_YELLOW(); break;
            case '5': LED_PURPLE(); break;
            case '6': LED_CYAN(); break;
            case '7': LED_WHITE(); break;
            case '8': LED_RGBOFF(); break;
        }
    }
}
```

## 3. 小结

理解了USART1的配置后，这章应该就简单了，我们再把我们之前写的led函数拿过来实现联动，具体就是：初始化led->初始化串口->主函数中调用两个函数就行啦，在串口收发的时候我们发送指令，不同指令对应不同led

---

2024.8.24 第一次修订，后期不再维护

2025.1.12 把内容简化，程序也改了很多
