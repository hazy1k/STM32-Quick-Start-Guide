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
