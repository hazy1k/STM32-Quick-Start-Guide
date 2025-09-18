#include "usart.h"

#define USARTx USART1
#define USART_GPIO GPIOA
#define USART_TX_PIN GPIO_Pin_9
#define USART_RX_PIN GPIO_Pin_10

void USARTx_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置TX引脚
    GPIO_InitStructure.GPIO_Pin = USART_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_GPIO, &GPIO_InitStructure);
    
    // 配置RX引脚
    GPIO_InitStructure.GPIO_Pin = USART_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART_GPIO, &GPIO_InitStructure);
    
    // USART配置
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTx, &USART_InitStructure);
    
    // 使能USART
    USART_Cmd(USARTx, ENABLE);
}

void USART_SendChar(char ch)
{
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(USARTx, (uint16_t)ch);
}

char USART_ReceiveChar(void)
{
    while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
    return (char)USART_ReceiveData(USARTx);
}

void USART_SendString(char *str)
{
    while(*str) {
        USART_SendChar(*str++);
    }
}

// 重定向printf到USART
int fputc(int ch, FILE *f)
{
    USART_SendChar(ch);
    return ch;
}
