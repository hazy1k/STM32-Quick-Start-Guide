#include "dma.h"

uint8_t ReceiveBuff[ReceBuff_Size]; // 发送缓冲区

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
