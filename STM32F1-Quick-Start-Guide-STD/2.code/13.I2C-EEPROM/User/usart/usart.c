#include "usart.h"
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
    USART_Cmd(USARTx, ENABLE);
}

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
