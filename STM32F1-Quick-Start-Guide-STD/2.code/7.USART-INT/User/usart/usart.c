#include "usart.h"

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

void USARTx_IRQHandler(void)
{
    uint8_t temp;
    if(USART_GetITStatus(USARTx, USART_IT_RXNE) != RESET) // 如果接收到数据
    {
        temp = USART_ReceiveData(USARTx); // 读取接收到的数据
        usart_sendbyte(USARTx, temp); // 回显接收到的数据
    }
}
