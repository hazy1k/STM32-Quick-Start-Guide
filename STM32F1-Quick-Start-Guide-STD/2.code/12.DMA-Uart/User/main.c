#include "stm32f10x.h"
#include "led.h"
#include "SysTick.h"
#include "usart.h"
#include "dma.h"
#include <stdio.h>
#include <string.h>

uint8_t Send_DMA_BUFFER[DMA_BUFFER_SIZE];

int main(void) 
{
    SysTick_Init();
    BSP_LED_Init();
    USARTx_Init();  // 初始化USART1
    
    // 1. 准备要发送的数据
    const char *message = "Hello DMA! Memory to USART transfer.\r\n";
    uint8_t msg_len = strlen(message);
    
    // 确保不超过缓冲区大小
    uint8_t copy_len = (msg_len < DMA_BUFFER_SIZE) ? msg_len : DMA_BUFFER_SIZE;
    memcpy(Send_DMA_BUFFER, message, copy_len);
    
    // 2. 初始化DMA
    USART_DMA_Init();
    
    // 3. 启用USART的DMA发送请求
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    
    // 4. 启动DMA传输
    DMA_StartTransfer();
    
    LED_ON(BLUE_LED_Pin);  // 开始传输，蓝灯亮
    
    // 5. 等待传输完成
    while(!DMA_IsTransferComplete()); 
    
    LED_OFF(BLUE_LED_Pin); // 传输完成，蓝灯灭
    LED_ON(GREEN_LED_Pin); // 绿灯亮表示成功
    
    // 6. 通过串口打印状态信息
    printf("DMA Transfer Complete!\r\n");
    printf("Sent %d bytes via DMA\r\n", copy_len);
    
    while(1) {
        // 可添加其他操作
    }
}
