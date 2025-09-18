#include "dma.h"
#include "lcd.h"
#include "usart.h"

DMA_HandleTypeDef  UART1TxDMA_Handler; // DMA句柄

// DMAx的各通道配置
// 这里的传输形式是固定的,这点要根据不同的情况来修改
// 从存储器->外设模式/8位数据宽度/存储器增量模式
// DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
// chx:DMA通道选择,@ref DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
{ 
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
        __HAL_RCC_DMA2_CLK_ENABLE();//DMA2时钟使能	
	}else 
	{
        __HAL_RCC_DMA1_CLK_ENABLE();//DMA1时钟使能 
	}
    
    __HAL_LINKDMA(&UART1_Handler,hdmatx,UART1TxDMA_Handler);    //将DMA与USART1联系起来(发送DMA)
    
    //Tx DMA配置
    UART1TxDMA_Handler.Instance=DMA_Streamx;                        // 数据流选择
    UART1TxDMA_Handler.Init.Channel=chx;                            // 通道选择
    UART1TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;         // 存储器到外设
    UART1TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;             // 外设非增量模式
    UART1TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                 // 存储器增量模式
    UART1TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;// 外设数据长度:8位
    UART1TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;   // 存储器数据长度:8位
    UART1TxDMA_Handler.Init.Mode=DMA_NORMAL;                        // 外设普通模式
    UART1TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;           // 中等优先级
    UART1TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
    UART1TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
    UART1TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;             // 存储器突发单次传输
    UART1TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;          // 外设突发单次传输
    HAL_DMA_DeInit(&UART1TxDMA_Handler);   
    HAL_DMA_Init(&UART1TxDMA_Handler);
} 

// 开启一次DMA传输
// huart:串口句柄
// pData：传输的数据指针
// Size:传输的数据量
void MYDMA_USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) // 函数参数：串口句柄，数据指针，数据长度
{
    HAL_DMA_Start(huart->hdmatx, (u32)pData, (uint32_t)&huart->Instance->DR, Size); // 开启DMA传输 
    huart->Instance->CR3 |= USART_CR3_DMAT;//使能串口DMA发送
}	  
